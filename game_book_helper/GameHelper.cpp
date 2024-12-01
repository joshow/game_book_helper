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
            std::string to;
            if (!(ss >> to))
            {
                err("input like: add [str]");
                continue;
            }
            add(cursor, to);
            show(findRoot(cursor));
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "memo")
        {
            std::string desc;
            ss.ignore();
            if (!std::getline(ss, desc))
            {
                memo(cursor);
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
        else if (cmd == "setclue")
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
                "add [str]        : ���� �ܶ����� n�ܶ����� �̵��ϴ� ���� �߰�.\n"
                "clue             : ��ϵ� ��� �ܼ��� �� ���, ���� �߰ߵ��� �ʾ����� -1.\n"
                "setclue [str][n] : str�ܼ��� n. ������ ��α� �� �˾Ƴ� ������ ��� �߰�.\n"
                "memo [str]       : ���� �ܶ��� ��ϵ� �޸� ���, ���� �ܶ��� str �޸� �߰�.\n"
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

void GameHelper::clue()
{
    if (clues.empty())
    {
        return;
    }
    for (const auto& clue : clues | std::views::drop(1))
    {
        std::print("{}: {}\n", clue.first, clue.second ? *clue.second : -1);
    }
    std::print("\n");
}

void GameHelper::setClue(const std::string& clue, int x)
{
    clues[clue] = x;
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

void GameHelper::memo(int id)
{
    std::print("{}\n", nodes[id].desc);
}

void GameHelper::addMemo(int id, const std::string& adding_desc)
{
    auto& desc = nodes[id].desc;
    desc += desc.empty() ? adding_desc : ", " + adding_desc;
}

void GameHelper::show(int id, const std::string& prefix, bool isLast, const std::string& log)
{
	const auto& node = nodes[id];

	std::print("{:20}\t\t{}\n",
		std::format("{}{}{}{}{}",
			prefix,
			(isLast ? "������" : "������"),
			log.empty() ? "" : std::format("({})=", log),
			node.id,
			cursor == node.id ? "��@" : node.needCheck ? "��" : ""),
		node.desc.empty() ? "" : "memo: " + node.desc);

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
			std::print("{:20}\t\t{}\n",
				std::format("{}{}{}",
					prefix + (isLast ? "   " : "��  "),
					(!--isNotLast ? "������" : "������"),
					std::format("({})=?", backlog)),
				node.desc.empty() ? "" : "memo: " + node.desc);
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
            std::string to;
            file >> to;
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
            file >> c >> x;
            setClue(c, x);
        }
        else if (cmd == "memo")
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
