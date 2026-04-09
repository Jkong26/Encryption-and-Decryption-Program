#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <cctype>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <sstream>

// Class for message processing
class MessageProcessor
{
private:
    std::string sanitizeMessage(const std::string &message, bool preserveSpaces = true)
    {
        std::string result;
        if (preserveSpaces)
        {
            for (char c : message)
            {
                if (std::isspace(c))
                    result.push_back('_');
                else
                    result.push_back(c);
            }
        }
        else
        {
            std::copy_if(message.begin(), message.end(),
                         std::back_inserter(result),
                         [](char c) { return !std::isspace(c); });
        }
        if (!result.empty() && result.back() != '.')
            result.push_back('.');
        return result;
    }

public:
    MessageProcessor() {}

    std::string prepareMessage(const std::string &message, bool preserveSpaces = true, bool isFirstRound = true)
    {
        std::string result;
        if (isFirstRound)
        {
            for (char c : message)
            {
                if (preserveSpaces && std::isspace(c))
                    result.push_back('_');
                else
                    result.push_back(c);
            }
            if (!result.empty() && result.back() != '.')
                result.push_back('.');
        }
        else
        {
            result = message;
        }
        return result;
    }

    char getRandomChar()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(65, 90);
        return static_cast<char>(dis(gen));
    }

    std::string findMessageEnd(const std::string &message)
    {
        size_t dotPos = message.find('.');
        if (dotPos != std::string::npos)
            return message.substr(0, dotPos + 1);
        return message;
    }
};

// Class for grid management
class GridManager
{
private:
    int gridSize;
    std::vector<std::vector<char>> grid;

    int calculateGridSize(const std::string &message)
    {
        int length = message.length();
        int size = 7;
        while (size * size < length)
            size += 2;
        return size;
    }

public:
    GridManager() : gridSize(0) {}

    void setGridSize(int size)
    {
        if (size < 3 || size % 2 == 0)
            throw std::invalid_argument("The grid size must be odd and greater than or equal to 3");
        gridSize = size;
        initializeGrid();
    }

    void setAutoGridSize(const std::string &message)
    {
        gridSize = calculateGridSize(message);
        initializeGrid();
    }

    void initializeGrid()
    {
        grid = std::vector<std::vector<char>>(gridSize, std::vector<char>(gridSize, ' '));
    }

    int getGridSize() const { return gridSize; }

    void setChar(int row, int col, char c)
    {
        if (row >= 0 && row < gridSize && col >= 0 && col < gridSize)
            grid[row][col] = c;
    }

    char getChar(int row, int col) const
    {
        if (row >= 0 && row < gridSize && col >= 0 && col < gridSize)
            return grid[row][col];
        return ' ';
    }

    void displayGrid() const
    {
        std::cout << std::string(gridSize * 4 + 1, '-') << std::endl;
        for (int i = 0; i < gridSize; i++)
        {
            std::cout << "| ";
            for (int j = 0; j < gridSize; j++)
                std::cout << grid[i][j] << " | ";
            std::cout << std::endl;
            std::cout << std::string(gridSize * 4 + 1, '-') << std::endl;
        }
    }

    std::string readByColumns() const
    {
        std::string result;
        for (int col = 0; col < gridSize; col++)
            for (int row = 0; row < gridSize; row++)
                result.push_back(grid[row][col]);
        return result;
    }

    std::vector<std::vector<char>> &getGrid() { return grid; }
};

// Main class for encryption and decryption
class DiamondCipher
{
private:
    MessageProcessor messageProcessor;
    GridManager gridManager;
    bool preserveSpaces;
    std::string originalMessage;

    void fillDiamondPattern(const std::string &message)
    {
        int size = gridManager.getGridSize();
        auto &grid = gridManager.getGrid();

        int center = size / 2, msgIdx = 0, msgLen = message.length();
        for (int layer = 0; layer <= center && msgIdx < msgLen; layer++)
        {
            std::vector<std::pair<int, int>> positions;
            if (layer == 0)
            {
                positions.emplace_back(center, center);
            }
            else
            {
                for (int step = 0; step <= layer; step++)
                    if (int r = center - layer + step, c = center + step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step <= layer; step++)
                    if (int r = center + step, c = center + layer - step + 1; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step <= layer; step++)
                    if (int r = center + layer - step + 1, c = center - step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step < layer; step++)
                    if (int r = center - step, c = center - layer + step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
            }
            for (auto &[r, c] : positions)
                if (msgIdx < msgLen)
                    grid[r][c] = message[msgIdx++];
        }

        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                if (grid[i][j] == ' ')
                    grid[i][j] = messageProcessor.getRandomChar();
    }

    std::string readDiamondPattern()
    {
        int size = gridManager.getGridSize();
        int center = size / 2;
        std::string result;

        for (int layer = 0; layer <= center; layer++)
        {
            std::vector<std::pair<int, int>> positions;
            if (layer == 0)
            {
                positions.emplace_back(center, center);
            }
            else
            {
                for (int step = 0; step <= layer; step++)
                    if (int r = center - layer + step, c = center + step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step <= layer; step++)
                    if (int r = center + step, c = center + layer - step + 1; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step <= layer; step++)
                    if (int r = center + layer - step + 1, c = center - step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
                for (int step = 1; step < layer; step++)
                    if (int r = center - step, c = center - layer + step; r >= 0 && r < size && c >= 0 && c < size)
                        positions.emplace_back(r, c);
            }
            for (auto &[r, c] : positions)
                result.push_back(gridManager.getChar(r, c));
        }
        return result;
    }

    std::string fixSpaces(const std::string &text)
    {
        std::string result;
        for (char c : text)
        {
            if (c == '_')
                result.push_back(' ');
            else
                result.push_back(c);
        }
        return result;
    }

public:
    DiamondCipher() : preserveSpaces(true) {}

    void setPreserveSpaces(bool preserve) { preserveSpaces = preserve; }
    bool getPreserveSpaces() const { return preserveSpaces; }

    std::string encryptOneRound(const std::string &message, bool manualGridSize = false, int size = 0, bool isFirstRound = true)
    {
        if (isFirstRound)
            originalMessage = message;

        std::string preparedMessage = messageProcessor.prepareMessage(message, preserveSpaces, isFirstRound);

        if (manualGridSize)
            gridManager.setGridSize(size);
        else
            gridManager.setAutoGridSize(preparedMessage);

        gridManager.initializeGrid();
        fillDiamondPattern(preparedMessage);

        return gridManager.readByColumns();
    }

    std::string decryptOneRound(const std::string &encryptedMessage)
    {
        int length = encryptedMessage.length();
        int size = static_cast<int>(std::sqrt(length) + 0.5);

        if (size * size != length || size % 2 == 0)
        {
            std::cout << "Error during decryption: The encrypted message is invalid. It must be the square of an odd number." << std::endl;
            return encryptedMessage;
        }

        gridManager.setGridSize(size);
        auto &grid = gridManager.getGrid();
        int index = 0;
        for (int col = 0; col < size; col++)
            for (int row = 0; row < size; row++)
                grid[row][col] = encryptedMessage[index++];

        std::string decryptedText = readDiamondPattern();
        if (preserveSpaces)
        {
            for (char &ch : decryptedText)
                if (ch == '_')
                    ch = ' ';
        }
        return decryptedText;
    }

    std::string encryptMultiRound(const std::string &message, int rounds)
    {
        if (rounds < 1)
            throw std::invalid_argument("The number of rounds must be greater than or equal to 1");

        originalMessage = message;
        std::string result = message;

        for (int i = 0; i < rounds; i++)
        {
            std::cout << "\nRound no. " << (i + 1) << ":" << std::endl;

            std::string preparedMessage;
            if (i == 0)
            {
                // First round: replace spaces with '_' and add a dot at the end
                preparedMessage = messageProcessor.prepareMessage(result, preserveSpaces, true);
            }
            else
            {
                // Subsequent rounds: use the message as-is (no space replacement)
                preparedMessage = messageProcessor.prepareMessage(result, false, false);
            }

            int gridSize = 7 + (i * 2);
            gridManager.setGridSize(gridSize);
            gridManager.initializeGrid();
            fillDiamondPattern(preparedMessage);
            gridManager.displayGrid();
            result = gridManager.readByColumns();
            std::cout << "Encrypted message after round " << (i + 1) << ": " << result << std::endl;
        }

        std::cout << "\nFinal encrypted message: " << result << std::endl;
        return result;
    }

    std::string decryptMultiRound(const std::string &encryptedMessage, int rounds)
    {
        if (rounds < 1)
            throw std::invalid_argument("Number of rounds must be >= 1");

        std::string result = encryptedMessage;

        for (int i = 0; i < rounds; ++i)
        {
            std::cout << "\nRound no. " << (i + 1) << ":\n";
            try
            {
                int length = result.length();
                int size;
                if (rounds == 1)
                {
                    int s = std::sqrt(length);
                    if (s * s != length || s % 2 == 0)
                    {
                        std::cout << "Warning: Adjusting inferred size.\n";
                        s = std::ceil(std::sqrt(length));
                        if (s % 2 == 0)
                            ++s;
                    }
                    size = s;
                }
                else
                {
                    size = 7 + ((rounds - 1 - i) * 2);
                }

                std::cout << "Debug - Using size: " << size << "\n";
                gridManager.setGridSize(size);
                auto &grid = gridManager.getGrid();
                int idx = 0;
                for (int col = 0; col < size; ++col)
                    for (int row = 0; row < size; ++row)
                        grid[row][col] = (idx < length) ? result[idx++] : ' ';

                gridManager.displayGrid();
                std::string decrypted = readDiamondPattern();

                if (i == rounds - 1)
                {
                    if (preserveSpaces)
                        for (char &ch : decrypted)
                            if (ch == '_')
                                ch = ' ';
                    if (auto dot = decrypted.find('.'); dot != std::string::npos)
                        decrypted = decrypted.substr(0, dot + 1);
                }

                std::cout << "Decrypted message after round " << (i + 1) << ": " << decrypted << "\n";
                result = decrypted;
            }
            catch (const std::exception &e)
            {
                std::cout << "Error during round " << (i + 1) << ": " << e.what() << "\n";
                break;
            }
        }

        std::cout << "\nFinal decrypted message: " << result << "\n";
        return result;
    }

    void displayCurrentGrid() const { gridManager.displayGrid(); }

    int getCurrentGridSize() const { return gridManager.getGridSize(); }
};

// Class for managing user interface functions
class UserInterface
{
private:
    DiamondCipher cipher;
    std::string currentMessage;
    std::string encryptedMessage;

    void displayMainMenu()
    {
        std::cout << std::string(24, '*') << std::endl;
        std::cout << "* Menu - Level 1       *" << std::endl;
        std::cout << "* Select an option:    *" << std::endl;
        std::cout << "* 1. Encrypt a message *" << std::endl;
        std::cout << "* 2. Decrypt a message *" << std::endl;
        std::cout << "* 3. Quit              *" << std::endl;
        std::cout << std::string(24, '*') << std::endl;
    }

    void displayEncryptionMenu()
    {
        std::cout << "\n" << std::endl;
        std::cout << std::string(40, '*') << std::endl;
        std::cout << "* Menu - Level 2: Encryption           *" << std::endl;
        std::cout << "* Select an option:                    *" << std::endl;
        std::cout << "* 1. Enter a message                   *" << std::endl;
        std::cout << "* 2. One-round encryption              *" << std::endl;
        std::cout << "* 3. Automatic multi-round encryption  *" << std::endl;
        std::cout << "* 4. Back                              *" << std::endl;
        std::cout << std::string(40, '*') << std::endl;
    }

    void displayOneRoundEncryptionMenu()
    {
        std::cout << "\n" << std::endl;
        std::cout << std::string(45, '*') << std::endl;
        std::cout << "* Menu - Level 3: Encryption                *" << std::endl;
        std::cout << "* Select an option:                         *" << std::endl;
        std::cout << "* 1. Enter a grid size                      *" << std::endl;
        std::cout << "* 2. Automatic grid size                    *" << std::endl;
        std::cout << "* 3. Print the grid and the encoded message *" << std::endl;
        std::cout << "* 4. Back                                   *" << std::endl;
        std::cout << std::string(45, '*') << std::endl;
    }

    void displayMultiRoundEncryptionMenu()
    {
        std::cout << "\n" << std::endl;
        std::cout << std::string(41, '*') << std::endl;
        std::cout << "* Menu - Level 3: Encryption            *" << std::endl;
        std::cout << "* Select an option:                     *" << std::endl;
        std::cout << "* 1. Enter the round number             *" << std::endl;
        std::cout << "* 2. For each round, print the grid and *" << std::endl;
        std::cout << "*    the corresponding encoded message  *" << std::endl;
        std::cout << "* 3. Back                               *" << std::endl;
        std::cout << std::string(41, '*') << std::endl;
    }

    void displayDecryptionMenu()
    {
        std::cout << "\n" << std::endl;
        std::cout << std::string(41, '*') << std::endl;
        std::cout << "* Menu - Level 2: Decryption            *" << std::endl;
        std::cout << "* Select an option:                     *" << std::endl;
        std::cout << "* 1. Enter a message                    *" << std::endl;
        std::cout << "* 2. Enter the round number             *" << std::endl;
        std::cout << "* 3. For each round, print the grid and *" << std::endl;
        std::cout << "*    the corresponding decoded message  *" << std::endl;
        std::cout << "* 4. Back                               *" << std::endl;
        std::cout << std::string(41, '*') << std::endl;
    }

    template <typename T>
    T getInput(const std::string &prompt)
    {
        std::string line;
        T value;
        while (true)
        {
            std::cout << prompt;
            std::getline(std::cin, line);
            if (line.empty())
            {
                std::cout << "Input cannot be empty. Please enter a number between 1 and 3." << std::endl;
                continue;
            }
            std::istringstream iss(line);
            if (iss >> value)
                return value;
            else
                std::cout << "The information is incorrect. Please try again." << std::endl;
        }
    }

    std::string getMessageInput(const std::string &prompt)
    {
        std::string message;
        std::cout << prompt;
        std::getline(std::cin, message);
        if (message.empty())
        {
            std::cout << "The message is empty. Please try again." << std::endl;
            // Recursive call to retry until a non-empty string is entered
            return getMessageInput(prompt);
        }
        return message;
    }

    void handleOneRoundEncryption()
    {
        bool back = false, manualGridSize = false;
        int gridSize = 0;
        while (!back)
        {
            displayOneRoundEncryptionMenu();
            int choice = getInput<int>("Enter your choice (1-4): ");
            switch (choice)
            {
            case 1:
                try
                {
                    gridSize = getInput<int>("Enter grid size (odd and >= 3): ");
                    manualGridSize = (gridSize >= 3 && gridSize % 2 != 0);
                    if (!manualGridSize)
                        std::cout << "Grid size must be odd and >= 3.\n";
                }
                catch (const std::exception &e)
                {
                    std::cout << "Error: " << e.what() << "\n";
                }
                break;
            case 2:
                manualGridSize = false;
                std::cout << "Automatic grid size selected.\n";
                if (!currentMessage.empty())
                    encryptedMessage = cipher.encryptOneRound(currentMessage);
                break;
            case 3:
                if (currentMessage.empty())
                {
                    std::cout << "Please enter text first.\n";
                    break;
                }
                try
                {
                    encryptedMessage = manualGridSize
                                           ? cipher.encryptOneRound(currentMessage, true, gridSize)
                                           : cipher.encryptOneRound(currentMessage);
                    std::cout << "Encrypted message: " << encryptedMessage << "\n";
                    std::cout << "Grid:\n";
                    cipher.displayCurrentGrid();
                }
                catch (const std::exception &e)
                {
                    std::cout << "Error: " << e.what() << "\n";
                }
                break;
            case 4:
                back = true;
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
            }
        }
    }

    void handleMultiRoundEncryption()
    {
        bool back = false;
        int rounds = 0;
        while (!back)
        {
            displayMultiRoundEncryptionMenu();
            int choice = getInput<int>("Enter your choice (1-3): ");
            switch (choice)
            {
            case 1:
                rounds = getInput<int>("Enter number of encryption rounds: ");
                if (rounds < 1)
                    std::cout << "Rounds must be >= 1.\n";
                break;
            case 2:
                if (currentMessage.empty())
                {
                    std::cout << "Please enter text first.\n";
                    break;
                }
                if (rounds < 1)
                {
                    std::cout << "Please set the number of rounds first.\n";
                    break;
                }
                try
                {
                    encryptedMessage = cipher.encryptMultiRound(currentMessage, rounds);
                }
                catch (const std::exception &e)
                {
                    std::cout << "Error: " << e.what() << "\n";
                }
                break;
            case 3:
                back = true;
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
            }
        }
    }

    void handleEncryption()
    {
        bool back = false;
        while (!back)
        {
            displayEncryptionMenu();
            int choice = getInput<int>("Enter your choice (1-4): ");
            switch (choice)
            {
            case 1:
                currentMessage = getMessageInput("Enter the text you want to encrypt: ");
                std::cout << "Message stored: " << currentMessage << "\n";
                cipher.setPreserveSpaces(true);
                break;
            case 2:
                if (currentMessage.empty())
                    std::cout << "Please enter text first.\n";
                else
                    handleOneRoundEncryption();
                break;
            case 3:
                if (currentMessage.empty())
                    std::cout << "Please enter text first.\n";
                else
                    handleMultiRoundEncryption();
                break;
            case 4:
                back = true;
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void handleDecryption()
    {
        bool back = false;
        int rounds = 0;
        std::string encryptedInput;
        while (!back)
        {
            displayDecryptionMenu();
            int choice = getInput<int>("Enter your choice (1-4): ");
            switch (choice)
            {
            case 1:
                encryptedInput = getMessageInput("Enter the encrypted message: ");
                std::cout << "Encrypted message stored.\n";
                break;
            case 2:
                rounds = getInput<int>("Enter the number of decryption rounds: ");
                if (rounds < 1)
                    std::cout << "Rounds must be >= 1.\n";
                else
                    std::cout << "Rounds set to " << rounds << ".\n";
                break;
            case 3:
                if (encryptedInput.empty())
                    std::cout << "Please enter the encrypted message first.\n";
                else if (rounds < 1)
                    std::cout << "Please set the number of rounds first.\n";
                else
                    try
                    {
                        cipher.decryptMultiRound(encryptedInput, rounds);
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << "Error: " << e.what() << "\n";
                    }
                break;
            case 4:
                back = true;
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
            }
        }
    }

public:
    UserInterface() {}

    void run()
    {
        bool quit = false;
        while (!quit)
        {
            displayMainMenu();
            int choice = getInput<int>("Enter your choice (1-3): ");
            switch (choice)
            {
            case 1:
                handleEncryption();
                break;
            case 2:
                handleDecryption();
                break;
            case 3:
                quit = true;
                std::cout << "Thank you for using the Diamond Cipher Program. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
            }
        }
    }
};

int main()
{
    try
    {
        UserInterface ui;
        ui.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "An unhandled error occurred: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred." << std::endl;
        return 2;
    }
    return 0;
}