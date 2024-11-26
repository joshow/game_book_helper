#include "GameHelper.h"

#include <ranges>
#include <fstream>
#include <print>
#include <iostream>
#include <algorithm>

constexpr std::string_view gameNames[] = { "�����ΰ�_��������_Ż��", "�ֵ���_������_Ż��", "10����_�����_������" };

bool isRoot(const node& x) {
    return x.id == x.parent;
}

std::vector<std::string> parse(std::string_view sv)
{
    return std::views::split(sv, '+') | std::ranges::to<std::vector<std::string>>();
}

GameHelper::GameHelper()
    : startedAt(std::chrono::system_clock::now())
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
    const auto duration = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now() - startedAt);
    std::print("\nGameHelper::������ ���ӳ�~ (���ӽð� : {})\n", duration);
}

int GameHelper::play()
{
    std::fstream file;
    file.open(saveFileName, std::ios::app);

    std::string line;

    while (std::getline(std::cin, line))
    {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "init")
        {
            int n;
            if (!(ss >> n))
            {
                err("input like: init [num]");
                continue;
            }
            init(n);
        }
        else if (cmd == "go")
        {
            int to;
            if (!(ss >> to))
            {
                err("input like: go [num]");
                continue;
            }
            go(to);
            show(findRoot(cursor));
        }
        else if (cmd == "add")
        {
            int to;
            if (!(ss >> to))
            {
                err("input like: add [num]");
                continue;
            }
            add(cursor, to);
            show(findRoot(cursor));
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "backlog")
        {
            std::string c;
            if (!(ss >> c))
            {
                err("input like: backlog [str]");
                continue;
            }
            backlog(c);
            continue;
        }
        else if (cmd == "addBacklog")
        {
            std::string to;
            if (!(ss >> to))
            {
                err("input like: addBacklog [str]");
                continue;
            }
            addBacklog(cursor, to);
        }
        else if (cmd == "memo")
        {
            memo(cursor);
            continue;
        }
        else if (cmd == "addmemo")
        {
            std::string desc;
            ss.ignore();
            if (!std::getline(ss, desc))
            {
                err("input like: addmemo [str]");
                continue;
            }
            addMemo(cursor, desc);
            memo(cursor);
        }
        else if (cmd == "clue")
        {
            clue();
            continue;
        }
        else if (cmd == "addclue")
        {
            std::string c;
            int x;
            if (!(ss >> c) || !(ss >> x))
            {
                err("input like: setclue [str] [num]");
                continue;
            }
            setClue(c, x);
            clue();
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
            std::print("init [n]         : �ִ� n������ �ִ� ���� �ʱ�ȭ.\n"
                "go [n]           : n���ܶ����� �̵�. n���� �����ܶ��� ���ٸ� root�ܶ�.\n"
                "add [n]          : ���� �ܶ����� n�ܶ����� �̵��ϴ� ���� �߰�.\n"
                "backlog [str]    : str�ܼ��� ������ ��α� ��� ���.\n"
                "addbacklog [str] : ��α� �߰�.\n"
                "clue             : ��ϵ� ��� �ܼ��� �� ���, ���� �߰ߵ��� �ʾ����� -1.\n"
                "setclue [str][n] : str�ܼ��� n. ������ ��α� �� �˾Ƴ� ������ ��� �߰�.\n"
                "memo             : ���� �ܶ��� ��ϵ� �޸� ���.\n"
                "addmemo [str]    : ���� �ܶ��� str �޸� �߰�.\n"
                "show             : ���� �ܶ��� ������¸� ���.\n"
                "showall          : ��� �ܶ� ������¸� ���.\n");
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

    file.close();

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

void GameHelper::go(int to)
{
    cursor = to;
    if (!nodes[to].parent)
    {
        nodes[to].parent = to;
    }
    setNeedCheck(to, false);
}

void GameHelper::add(int from, int to)
{
    nodes[to].parent = from;
    nodes[from].childs.insert(to);
    setNeedCheck(to, true);
}

void GameHelper::setNeedCheck(int id, bool check)
{
    nodes[id].needCheck = check;
}

void GameHelper::backlog(const std::string& clue)
{
    for (const auto backlog : backlogs[clue])
    {
        std::print("{}��{} ", backlog.first, backlog.second);
    }
    std::print("\n");
}

void GameHelper::addBacklog(int from, const std::string& to)
{
    for (auto token : parse(to))
    {
        try {
            std::stoi(token);
        }
        catch (std::invalid_argument) {
            backlogs[token].emplace_back(from, to);
            clues[token] = std::nullopt;
        }
    }
}

void GameHelper::clue()
{
    if (clues.empty())
    {
        return;
    }
    std::print("{}:{}", clues.begin()->first, clues.begin()->second ? *clues.begin()->second : -1);
    for (const auto& clue : clues | std::views::drop(1))
    {
        std::print(", {}:{}", clue.first, clue.second ? *clue.second : -1);
    }
    std::print("\n");
}

void GameHelper::setClue(const std::string& clue, int x)
{
    clues[clue] = x;

    for (const auto backlog : backlogs[clue])
    {
        int sum = 0;
        if (std::ranges::all_of(parse(backlog.second), [&sum, this](const auto token) {
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
            add(backlog.first, sum);
            std::print("{}��{}:{} added.\n", backlog.first, backlog.second, sum);
        }
    }
}

void GameHelper::memo(int id)
{
    std::print("{}\n", nodes[id].desc);
}

void GameHelper::addMemo(int id, const std::string& adding_desc)
{
    auto& desc = nodes[id].desc;
    desc += desc.empty() ? adding_desc : ", " + adding_desc;
}

void GameHelper::show(int id, const std::string& prefix, bool isLast)
{
    const auto& node = nodes[id];

    std::print("{}{}{}{}\t\t{}\n",
        prefix,
        (isLast ? "������" : "������"),
        id,
        cursor == id ? "��@" : nodes[id].needCheck ? "��" : "",
        node.desc.empty() ? "" : "memo: " + node.desc);

    for (int i = 0; int child : node.childs) {
        show(child, prefix + (isLast ? "   " : "��  "), ++i == node.childs.size());
    }
}

void GameHelper::showAll()
{
    for (auto node : nodes | std::views::drop(1) | std::views::filter(isRoot))
    {
        show(node.id);
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

void GameHelper::load()
{
    std::fstream file;
    file.open(saveFileName, std::ios::in);

    std::string cmd;

    while (file >> cmd)
    {
        if (cmd == "init")
        {
            int n;
            file >> n;
            init(n);
        }
        else if (cmd == "go")
        {
            int to;
            file >> to;
            go(to);
        }
        else if (cmd == "add")
        {
            int to;
            file >> to;
            add(cursor, to);
        }
        else if (cmd == "addbacklog")
        {
            std::string to;
            file >> to;
            addBacklog(cursor, to);
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "setclue" || cmd == "addclue")
        {
            std::string c;
            int x;
            file >> c >> x;
            setClue(c, x);
        }
        else if (cmd == "addmemo")
        {
            std::string desc;
            file.ignore();
            std::getline(file, desc);
            addMemo(cursor, desc);
        }
    }

    std::print("GameHelper::loading completed!\n");

    showAll();

    file.close();
}
