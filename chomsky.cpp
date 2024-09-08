#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

// Função para verificar se uma string é terminal
bool isTerminal(const string &symbol, const unordered_set<string> &terminals) {
    return terminals.find(symbol) != terminals.end();
}

// Função para converter uma gramática para a Forma Normal de Chomsky
void convertToChomskyNormalForm(
    unordered_set<string> &terminals,
    unordered_set<string> &nonTerminals,
    string &startSymbol,
    vector<pair<string, string>> &productions
) {
    // Mapeia terminais para novos não-terminais como T_a, T_b, etc.
    unordered_map<string, string> terminalMappings;

    // Passo 1: Substituir terminais por novos não-terminais
    for (auto &production : productions) {
        string lhs = production.first;
        string rhs = production.second;

        string new_rhs; // Nova string para o lado direito da produção

        // Construir o novo lado direito substituindo terminais
        for (size_t i = 0; i < rhs.length(); ++i) {
            string symbol(1, rhs[i]);
            if (isTerminal(symbol, terminals)) {
                if (terminalMappings.find(symbol) == terminalMappings.end()) {
                    // Cria um novo não-terminal para cada terminal, no formato T_a, T_b, etc.
                    string newNonTerminal = "T_" + symbol;
                    terminalMappings[symbol] = newNonTerminal;
                    nonTerminals.insert(newNonTerminal); // Adiciona ao conjunto de não-terminais
                }
                new_rhs += terminalMappings[symbol]; // Adicionar o não-terminal correspondente
            } else {
                new_rhs += symbol; // Adicionar o símbolo não-terminal original
            }
        }
        production.second = new_rhs; // Atualizar o lado direito da produção
    }

    // Passo 2: Dividir produções que têm mais de dois símbolos no lado direito
    vector<pair<string, string>> newProductions;
    int newVarCount = 1;

    for (auto &production : productions) {
        string lhs = production.first;
        string rhs = production.second;

        // Aqui, vamos dividir rhs em símbolos (não caracteres)
        vector<string> symbols;
        for (size_t i = 0; i < rhs.length();) {
            if (rhs[i] == 'T' && i + 2 < rhs.length() && rhs[i + 1] == '_') {
                // Encontrou um terminal substituído, como T_a
                symbols.push_back(rhs.substr(i, 3)); // Adiciona o símbolo completo T_a
                i += 3;
            } else {
                // Caso contrário, trata o símbolo normalmente
                symbols.push_back(string(1, rhs[i]));
                i++;
            }
        }

        // Agora verifica se o lado direito tem mais de 2 símbolos
        if (symbols.size() > 2) {
            string prevVar = lhs;
            for (size_t i = 0; i < symbols.size() - 2; ++i) {
                string newVar = "X" + to_string(newVarCount++); // Criar novo não-terminal intermediário
                nonTerminals.insert(newVar);
                newProductions.push_back({prevVar, symbols[i] + newVar});
                prevVar = newVar;
            }
            newProductions.push_back({prevVar, symbols[symbols.size() - 2] + symbols[symbols.size() - 1]});
        } else {
            newProductions.push_back(production);
        }
    }

    productions = newProductions;

    // Adicionar as novas produções para os terminais
    for (auto &entry : terminalMappings) {
        productions.push_back({entry.second, entry.first});
    }
}


// Função para carregar o arquivo de entrada
void loadGrammar(const string &filename,unordered_set<string> &terminals,unordered_set<string> &nonTerminals,string &startSymbol,vector<pair<string, string>> &productions) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Erro ao abrir arquivo de entrada!" << endl;
        exit(1);
    }

    string line;
    // Linha 1: Número total de símbolos terminais
    getline(infile, line);

    // Linha 2: Símbolos terminais
    getline(infile, line);
    vector<string> terminalSymbols = split(line, ',');
    for (const string &t : terminalSymbols) {
        terminals.insert(t);
    }

    // Linha 3: Número total de símbolos não-terminais
    getline(infile, line);

    // Linha 4: Símbolos não-terminais
    getline(infile, line);
    vector<string> nonTerminalSymbols = split(line, ',');
    for (const string &nt : nonTerminalSymbols) {
        nonTerminals.insert(nt);
    }

    // Linha 5: Símbolo inicial
    getline(infile, line);
    startSymbol = line;

    // Linha 6: Número total de produções
    getline(infile, line);

    // Linhas seguintes: Produções
    while (getline(infile, line)) {
        size_t arrowPos = line.find("->");
        string lhs = line.substr(0, arrowPos - 1);
        string rhs = line.substr(arrowPos + 3);
        productions.push_back({lhs, rhs});
    }

    infile.close();
}

// Função para salvar o arquivo de saída
void saveGrammar(const string &filename,
                 const unordered_set<string> &terminals,
                 const unordered_set<string> &nonTerminals,
                 const string &startSymbol,
                 const vector<pair<string, string>> &productions) {
    ofstream outfile(filename);
    if (!outfile) {
        cerr << "Erro ao abrir arquivo de saída!" << endl;
        exit(1);
    }

    // Linha 1: Número total de símbolos terminais
    outfile << terminals.size() << endl;

    // Linha 2: Símbolos terminais (imprime na mesma linha, separado por vírgula)
    bool first = true;
    for (const auto &t : terminals) {
        if (!first) {
            outfile << ",";
        }
        outfile << t;
        first = false;
    }
    outfile << endl; // Garantir que há uma quebra de linha no final da lista de terminais

    // Linha 3: Número total de símbolos não-terminais
    outfile << nonTerminals.size() << endl;

    // Linha 4: Símbolos não-terminais (imprime na mesma linha, separado por vírgula)
    first = true;
    for (const auto &nt : nonTerminals) {
        if (!first) {
            outfile << ",";
        }
        outfile << nt;
        first = false;
    }
    outfile << endl; // Garantir que há uma quebra de linha no final da lista de não-terminais

    // Linha 5: Símbolo inicial
    outfile << startSymbol << endl;

    // Linha 6: Número total de produções
    outfile << productions.size() << endl;

    // Linhas seguintes: Produções
    for (const auto &production : productions) {
        outfile << production.first << " -> " << production.second << endl;
    }

    outfile.close();
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <arquivo_entrada.txt> <arquivo_saida.txt>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    unordered_set<string> terminals;
    unordered_set<string> nonTerminals;
    string startSymbol;
    vector<pair<string, string>> productions;

    // Carregar gramática do arquivo de entrada
    loadGrammar(inputFile, terminals, nonTerminals, startSymbol, productions);

    // Converter para Forma Normal de Chomsky
    convertToChomskyNormalForm(terminals, nonTerminals, startSymbol, productions);

    // Salvar gramática no arquivo de saída
    saveGrammar(outputFile, terminals, nonTerminals, startSymbol, productions);

    cout << "Gramática convertida para Forma Normal de Chomsky e salva em: " << outputFile << endl;

    return 0;
}
