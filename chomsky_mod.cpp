#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

using namespace std;

// Função para dividir uma string por um delimitador
vector<string> split(const string &str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Função auxiliar para remover espaços em branco ao redor de uma string
string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Função para lidar com produções alternativas com "|"
vector<pair<string, string>> handleAlternatives(const string &lhs, const string &rhs) {
    vector<pair<string, string>> productions;
    vector<string> alternatives = split(rhs, '|'); // Divide as alternativas por "|"
    for (const auto &alt : alternatives) {
        productions.push_back(make_pair(lhs, trim(alt))); // Adiciona cada alternativa como uma produção separada
    }
    return productions;
}

// Modificação na leitura das produções na função loadGrammar
void loadGrammar(const string &filename,vector<string> &terminals,vector<string> &nonTerminals,string &startSymbol,vector<pair<string, string>> &productions) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Erro ao abrir arquivo de entrada!" << endl;
        exit(1);
    }

    string line;
    getline(infile, line); // Número total de símbolos terminais
    getline(infile, line); // Símbolos terminais
    vector<string> terminalSymbols = split(line, ',');
    terminals.insert(terminals.end(), terminalSymbols.begin(), terminalSymbols.end());

    getline(infile, line); // Número total de símbolos não-terminais
    getline(infile, line); // Símbolos não-terminais
    vector<string> nonTerminalSymbols = split(line, ',');
    nonTerminals.insert(nonTerminals.end(), nonTerminalSymbols.begin(), nonTerminalSymbols.end());

    getline(infile, line); // Símbolo inicial
    startSymbol = line;

    getline(infile, line); // Número total de produções
    while (getline(infile, line)) {
        size_t arrowPos = line.find("->");
        string lhs = trim(line.substr(0, arrowPos - 1));
        string rhs = trim(line.substr(arrowPos + 3));
        vector<pair<string, string>> newProductions = handleAlternatives(lhs, rhs);
        productions.insert(productions.end(), newProductions.begin(), newProductions.end());
    }

    infile.close();
}

// Função para salvar o arquivo de saída
void saveGrammar(const string &filename,const vector<string> &terminals,const vector<string> &nonTerminals,const string &startSymbol,const vector<pair<string, string>> &productions) {
    ofstream outfile(filename);
    if (!outfile) {
        cerr << "Erro ao abrir arquivo de saída!" << endl;
        exit(1);
    }

    // Linha 1: Número total de símbolos terminais
    outfile << terminals.size() << endl;

    // Linha 2: Símbolos terminais (imprime na mesma linha, separado por vírgula)
    for (size_t i = 0; i < terminals.size(); ++i) {
        if (i > 0) {
            outfile << ",";
        }
        outfile << terminals[i];
    }
    outfile << endl;

    // Linha 3: Número total de símbolos não-terminais
    outfile << nonTerminals.size() << endl;

    // Linha 4: Símbolos não-terminais (imprime na mesma linha, separado por vírgula)
    for (size_t i = 0; i < nonTerminals.size(); ++i) {
        if (i > 0) {
            outfile << ",";
        }
        outfile << nonTerminals[i];
    }
    outfile << endl; // Garante que todos os não-terminais sejam impressos na mesma linha

    // Linha 5: Símbolo inicial
    outfile << startSymbol << endl;

    // Linha 6: Número total de produções
    outfile << productions.size() << endl;

    vector<pair<string, string>> copProductions;
    copy(productions.begin(),productions.end(),back_inserter(copProductions));
    // Linhas seguintes: Produções
    for (int k=0; k <=copProductions.size(); k++) {
        for(int j=k; j <copProductions.size(); j++) {
            if(k<j){
                if(copProductions[j].first == copProductions[k].first){
                    copProductions[k].second = copProductions[k].second + "|" + copProductions[j].second;
                    copProductions.erase(copProductions.begin()+j);
                }
            }
        }
    }
    for (size_t i=0; i <copProductions.size(); i++) {
        outfile << copProductions[i].first << " -> " << copProductions[i].second << endl;
    }

    outfile.close();
}

// Função para converter uma gramática para a Forma Normal de Chomsky
void convertToChomskyNormalForm(
    vector<string> &terminals,
    vector<string> &nonTerminals,
    string &startSymbol,
    vector<pair<string, string>> &productions
) {
    unordered_map<string, string> terminalMappings;

    for (auto &production : productions) {
        string lhs = production.first;
        string rhs = production.second;

        string new_rhs;

        // Substituição de terminais por novos não-terminais
        for (size_t i = 0; i < rhs.length(); ++i) {
            string symbol(1, rhs[i]);
            if (find(terminals.begin(), terminals.end(), symbol) != terminals.end() && rhs.length()>1) {
                if (terminalMappings.find(symbol) == terminalMappings.end()) {
                    // Adicionar novo não-terminal correspondente, como T_a
                    string newNonTerminal = "T_" + symbol;
                    terminalMappings[symbol] = newNonTerminal;
                    if (find(nonTerminals.begin(), nonTerminals.end(), newNonTerminal) == nonTerminals.end()) {
                        nonTerminals.push_back(newNonTerminal);
                    }
                }
                new_rhs += terminalMappings[symbol];
            } else {
                new_rhs += symbol;
            }
        }
        production.second = new_rhs;
    }

    vector<pair<string, string>> newProductions;
    int newVarCount = 1;

    // Dividir as produções com mais de dois símbolos no lado direito
    for (auto &production : productions) {
        string lhs = production.first;
        string rhs = production.second;

        vector<string> symbols;
        for (size_t i = 0; i < rhs.length();) {
            if (rhs[i] == 'T' && i + 2 < rhs.length() && rhs[i + 1] == '_') {
                symbols.push_back(rhs.substr(i, 3));
                i += 3;
            } else {
                symbols.push_back(string(1, rhs[i]));
                i++;
            }
        }

        if (symbols.size() > 2) {
            string prevVar = lhs;
            for (size_t i = 0; i < symbols.size() - 2; ++i) {
                string newVar = "X" + to_string(newVarCount++);
                nonTerminals.push_back(newVar);
                newProductions.push_back(make_pair(prevVar, symbols[i] + newVar));
                prevVar = newVar;
            }
            newProductions.push_back(make_pair(prevVar, symbols[symbols.size() - 2] + symbols[symbols.size() - 1]));
        } else {
            newProductions.push_back(production);
        }
    }

    productions = newProductions;

    for (auto &entry : terminalMappings) {
        productions.push_back(make_pair(entry.second, entry.first));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <arquivo_entrada.txt> <arquivo_saida.txt>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    vector<string> terminals;
    vector<string> nonTerminals;
    string startSymbol;
    vector<pair<string, string>> productions;

    loadGrammar(inputFile, terminals, nonTerminals, startSymbol, productions);
    convertToChomskyNormalForm(terminals, nonTerminals, startSymbol, productions);
    saveGrammar(outputFile, terminals, nonTerminals, startSymbol, productions);

    cout << "Gramática convertida para Forma Normal de Chomsky e salva em: " << outputFile << endl;

    return 0;
}
