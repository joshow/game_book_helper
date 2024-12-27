#include "GameHelper.h"

#include <ranges>
#include <fstream>
#include <print>
#include <iostream>
#include <functional>
#include <algorithm>

constexpr std::string_view gameNames[] = { "�����ΰ�_��������_Ż��", "�ֵ���_������_Ż��", "10����_�����_������" };

bool isRoot(const node& x) {
    return x.id == x.parent;
}

std::vector<std::string> parse(std::string_view sv)
{
    return std::views::split(sv, '+') | std::ranges::to<std::vector<std::string>>();
}

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
}

GameHelper::GameHelper()
	: startedAt(std::chrono::system_clock::now()), consoleHandle(GetStdHandle(STD_OUTPUT_HANDLE))
{
    for (int i = 0; i < 3; ++i)
    {
        std::print("{}. {}\n", i, gameNames[i]);
    }
    std::print("���� ��ȣ�� �����ϼ���: ");
    int n;
    std::cin >> n;

    saveFileName = std::format("saveFiles/{}.txt", gameNames[n]);

    load();
}

GameHelper::~GameHelper()
{
}

int GameHelper::play()
{
    std::fstream file;
    file.open(saveFileName, std::ios::app);

    std::string line;

    while (std::print("\n===================================\n\n"), std::getline(std::cin, line))
    {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        std::print("\n");

        if (cmd == "init")
        {
            int n;
            if (!(ss >> n))
            {
                err("input like: init n");
                continue;
            }
            init(n);
        }
        else if (cmd == "root")
        {
            int id;
            std::string memo;
            if (!(ss >> id) || ss.ignore(), !std::getline(ss, memo))
            {
                err("input like: root id memo");
                continue;
            }
            setRoot(id, memo);
        }
        else if (cmd == "go")
        {
            int id;
            if (!(ss >> id))
            {
                err("input like: go id");
                continue;
            }
            go(id);
            show(findRoot(cursor));
        }
        else if (cmd == "add")
        {
            std::string id;
            if (!(ss >> id))
            {
                err("input like: add id|backlog");
                continue;
            }
            add(cursor, id);
            show(findRoot(cursor));
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "memo")
        {
            std::string memo;
            ss.ignore();
            if (!std::getline(ss, memo))
            {
                continue;
            }
            addMemo(cursor, memo);
            show(cursor);
        }
        else if (cmd == "clue")
        {
            showClue();
            continue;
        }
        else if (cmd == "setclue")
        {
            std::string c;
            int x;
            if (!(ss >> c) || !(ss >> x))
            {
                err("input like: setclue clue x");
                continue;
            }
            setClue(c, x);
            showClue();
        }
        else if (cmd == "show")
        {
            show(findRoot(cursor));
            continue;
        }
        else if (cmd == "showall")
        {
            showAll();
            continue;
        }
        else if (cmd == "help")
        {
            std::print("init n           : �ִ� n������ �ִ� ���� �ʱ�ȭ.\n"
                "go id            : n���ܶ����� �̵�. n���� �����ܶ��� ���ٸ� root�ܶ�.\n"
                "add id|backlog   : ���� �ܶ����� n�ܶ����� �̵��ϴ� ���� �߰�.\n"
                "clue             : ��ϵ� ��� �ܼ��� �� ���, ���� �߰ߵ��� �ʾ����� -1.\n"
                "setclue clue x   : str�ܼ��� n. ������ ��α� �� �˾Ƴ� ������ ��� �߰�.\n"
                "memo [memo]      : ���� �ܶ��� ��ϵ� �޸� ���, ���� �ܶ��� str �޸� �߰�.\n"
                "show             : ���� �ܶ��� ������¸� ���.\n"
                "needcheck        : ���� �ܶ��� Ȯ�� �ʿ��� ���·� ����\n"
                "showall          : ��� �ܶ� ������¸� ���.\n"
                "quit             : ���� ����\n");
            continue;
        }
        else if (cmd == "quit")
        {
            break;
        }
        else
        {
            err("invalid command.");
            continue;
        }

        file << line << std::endl;
    }

    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startedAt);
    totalPlayTime += duration;
    file << std::format("playtime {}", duration.count()) << std::endl;

    file.close();

    std::print("GameHelper::������ ���ӳ�~\n���� ���ӽð� : {}\n���� ���ӽð� : {}\n",
        std::chrono::duration_cast<std::chrono::minutes>(duration),
        std::chrono::duration_cast<std::chrono::minutes>(totalPlayTime));

    return 0;
}

void GameHelper::err(std::string_view msg)
{
    std::cerr << "[error] " << msg << std::endl;
}

void GameHelper::init(int n)
{
    nodes = std::vector<node>(n + 5);
    for (int i : std::views::iota(0, n + 5))
    {
        nodes[i].id = i;
    }
}

void GameHelper::setRoot(int id, const std::string& memo)
{
    nodes[id].parent = id;
    nodes[id].memo = { memo };
    setNeedCheck(id, true);
}

void GameHelper::go(int id)
{
    cursor = id;
    if (!nodes[id].parent)
    {
        nodes[id].parent = id;
    }
    setNeedCheck(id, false);
}

void GameHelper::add(int from, const std::string& to)
{
    if (std::ranges::all_of(to, isdigit))
    {
        const auto ito = std::stoi(to);
        nodes[from].childs.insert(ito);
        nodes[ito].parent = from;
        setNeedCheck(ito, true);
    }
    else
    {
        nodes[from].backlogs.insert(to);
        if (const auto sum = trySum(to); sum)
        {
            nodes[*sum].parent = from;
            setNeedCheck(*sum, true);
        }
    }
}

void GameHelper::setNeedCheck(int id, bool check)
{
    nodes[id].needCheck = check;
}

void GameHelper::showClue()
{
    std::vector<std::pair<std::string, std::optional<int>>> v;
    v.assign_range(clues);
    std::ranges::sort(v, [](const auto& l, const auto& r) {
        if (l.first.starts_with("�ð�") != r.first.starts_with("�ð�"))
        {
            return l.first.starts_with("�ð�");
        }
        return l.first < r.first;
        });
    for (const auto& clue : v)
    {
        std::print("{}: {}\n", clue.first, clue.second ? *clue.second : -1);
    }
}

void GameHelper::setClue(const std::string& showClue, int x)
{
    clues[showClue] = x;
    for (const auto& node : nodes)
    {
        for (const auto& backlog : node.backlogs)
        {
            if (const auto sum = trySum(backlog); sum)
            {
                if (!nodes[*sum].parent) // ���� �߰ߵ�
                {
                    nodes[*sum].parent = node.id;
                    setNeedCheck(*sum, true);
                }
            }
        }
    }
}

void GameHelper::addMemo(int id, const std::string& adding_desc)
{
    nodes[id].memo.push_back(adding_desc);
}

void GameHelper::show(int id, const std::string& prefix, bool isLast, const std::string& log)
{
    const auto& node = nodes[id];

    if (isRoot(node) && !node.memo.empty())
    {
        std::print("# {}\n", node.memo.front());
    }

	SetConsoleTextAttribute(consoleHandle, 7);
	std::print("{}{}",
		prefix,
		(isLast ? "������" : "������"));

	SetConsoleTextAttribute(consoleHandle, cursor == node.id ? 10 : node.needCheck ? 4 : 7);
	std::print("{}{}{}", 
        log.empty() ? "" : std::format("({})=", log),
        node.id,
        cursor == node.id ? "��@" : node.needCheck ? "��" : "");

	SetConsoleTextAttribute(consoleHandle, 7);
    if (node.memo.empty())
    {
        std::print("\n");
    }
    else
    {
        for (int i = 0; const auto & memo : node.memo)
        {
            for (int j = 0; const auto& chunk : memo | std::views::chunk(60))
            {
                if (i > 0 || j > 0)
                {
				    std::print("{}{}{}", prefix, (isLast ? "   " : "��  "), node.childs.empty() && node.backlogs.empty() ? "" : "��  ");
                }
                SetConsoleCursorPosition(consoleHandle, COORD{ 40, GetConsoleCursorPosition(consoleHandle).Y });
                if (i == 0)
                {
                    std::print("[{}] {}. {:40}\n", node.id, ++i, std::string_view(chunk));
                }
                else if (j == 0)
                {
                    std::print("{}{}. {}\n", std::string(std::format("[{}] ", node.id).size(), ' '), ++i, std::string_view(chunk));
                }
                else
                {
                    std::print("{}{}\n", std::string(std::format("[{}] {}. ", node.id, i).size(), ' '), std::string_view(chunk));
                }
                j++;
            }
        }
    }

    int isNotLast = node.childs.size() + node.backlogs.size();
    for (int id : node.childs)
    {
        show(id, prefix + (isLast ? "   " : "��  "), !--isNotLast);
    }
    for (const std::string& backlog : node.backlogs)
    {
        if (const auto sum = trySum(backlog); sum)
        {
            show(*sum, prefix + (isLast ? "   " : "��  "), !--isNotLast, backlog);
        }
        else
        {
			std::print("{}{}{}\n",
				prefix + (isLast ? "   " : "��  "),
				(!--isNotLast ? "������" : "������"),
				std::format("({})=?", backlog));
        }
    }
}

void GameHelper::showAll()
{
    for (auto id : std::views::iota(1u, nodes.size()))
    {
        if (isRoot(nodes[id]))
        {
            show(id);
        }
    }
}

int GameHelper::findRoot(int id)
{
    while (nodes[id].parent != id)
    {
        id = nodes[id].parent;
    }
    return id;
}

std::optional<int> GameHelper::trySum(const std::string& log)
{
    int sum = 0;
    if (!std::ranges::all_of(parse(log), [&sum, this](const auto token) {
        try {
            sum += std::stoi(token);
            return true;
        }
        catch (std::invalid_argument) {
            if (clues[token].has_value())
            {
                sum += *clues[token];
                return true;
            }
        }
        return false;
        }))
    {
        return std::nullopt;
    }
    return sum;
}

void GameHelper::load()
{
    std::fstream file;
    file.open(saveFileName, std::ios::in);

    std::string line;

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string cmd;

        ss >> cmd;

        if (cmd == "init")
        {
            int n;
            ss >> n;
            init(n);
        }
        else if (cmd == "root")
        {
            int id;
            std::string memo;
            ss >> id;
            ss.ignore();
            std::getline(ss, memo);
            setRoot(id, memo);
        }
        else if (cmd == "go")
        {
            int to;
            ss >> to;
            go(to);
        }
        else if (cmd == "add")
        {
            std::string to;
            ss >> to;
            add(cursor, to);
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "setclue")
        {
            std::string c;
            int x;
            ss >> c >> x;
            setClue(c, x);
        }
        else if (cmd == "memo")
        {
            std::string memo;
            ss.ignore();
            std::getline(ss, memo);
            addMemo(cursor, memo);
        }
        else if (cmd == "playtime")
        {
            int m;
            ss >> m;
            totalPlayTime += std::chrono::seconds(m);
        }
    }

    std::print("GameHelper::���̺����� �ε� �Ϸ�!\n\n{} ���� ����~!\n", cursor);

    file.close();
}
