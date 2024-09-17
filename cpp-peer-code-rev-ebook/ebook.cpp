#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "test_runner.h"

using namespace std::literals;

struct CMD {
    std::string CommandType;
    int user_id;
    int page_num;
};

namespace BookStat {

    const unsigned int MAX_USERS = 100000 + 1;
    const unsigned int MAX_PAGES = 1000 + 1;

    unsigned total_users = 0;

    std::vector<int> users;
    std::vector<int> pages;

    void Init() {
        users.resize(MAX_USERS);
        std::fill(users.begin(), users.end(), -1);
        pages.resize(MAX_PAGES);
        std::fill(pages.begin(), pages.end(), 0);
        total_users = 0;
    }

    void ProcessCmdREAD(int user_id, int page_num) {

        if (users[user_id] < 0) {
            total_users++;
        }
        else {
            int old_page_num = users[user_id];
            pages[old_page_num] -= 1;
        }
        users[user_id] = page_num;
        pages[page_num] += 1;
    }

    double Cheer(int user_id) {

        double users_part = -1.0f;

        if (users[user_id] < 0) {
            users_part = 0.0f;
        }
        else if (total_users == 1) {
            users_part = 1.0f;
        }
        else {
            auto page_num = users[user_id];
            int user_count = 0;
            for (int i = 0; i < page_num; ++i) {
                user_count += pages[i];
            }
            users_part = 1.0f;
            if (total_users > 1) {
                users_part = static_cast<double> (user_count) / static_cast<double> (total_users - 1);
            }
        }

        return users_part;
    }

    void ProcessCmdCHEER(int user_id) {

        auto users_part = Cheer(user_id);

        if (users_part < 0.0f) {
            throw "error "s+ std::to_string(users_part)+" "s+std::to_string(total_users);
        }

        std::cout << std::setprecision(6) << users_part << std::endl;
    }
}

void TestAddUser() {
    BookStat::Init();
    ASSERT_EQUAL(BookStat::Cheer(1), 0.0);
    BookStat::ProcessCmdREAD(1, 1);
    ASSERT_EQUAL(BookStat::Cheer(1), 1.0);
}

void TestAddUserButCheerNotExisted() {
    BookStat::Init();
    ASSERT_EQUAL(BookStat::Cheer(1), 0.0);
    BookStat::ProcessCmdREAD(1, 1);
    ASSERT_EQUAL(BookStat::Cheer(2), 0.0);
}

void TestReadTwice() {
    BookStat::Init();
    BookStat::ProcessCmdREAD(1, 1);
    ASSERT_EQUAL(BookStat::Cheer(1), 1.0);
    BookStat::ProcessCmdREAD(1, 2);
    ASSERT_EQUAL(BookStat::Cheer(1), 1.0);
}

void  TestBetterThanThird() {
    BookStat::Init();
    BookStat::ProcessCmdREAD(1, 1);
    BookStat::ProcessCmdREAD(2, 2);
    BookStat::ProcessCmdREAD(3, 3);
    ASSERT_EQUAL(BookStat::Cheer(2), 0.5);
    BookStat::ProcessCmdREAD(4, 4);
    ASSERT_EQUAL(BookStat::Cheer(2), 1.0 / 3.0);
}

void TestBetterThan2Third() {
    BookStat::Init();
    BookStat::ProcessCmdREAD(1, 1);
    BookStat::ProcessCmdREAD(2, 2);
    BookStat::ProcessCmdREAD(3, 3);
    ASSERT_EQUAL(BookStat::Cheer(2), 0.5);
    BookStat::ProcessCmdREAD(4, 4);
    ASSERT_EQUAL(BookStat::Cheer(3), 2.0 / 3.0);
}


void TestMaxID() {
    const int MAX_USER_COUNT = 100'000;
    BookStat::Init();
    ASSERT_EQUAL(BookStat::Cheer(MAX_USER_COUNT), 0.0);
    BookStat::ProcessCmdREAD(MAX_USER_COUNT, 1);
    ASSERT_EQUAL(BookStat::Cheer(MAX_USER_COUNT), 1.0);
}

void TestMaxPage() {
    BookStat::Init();
    ASSERT_EQUAL(BookStat::Cheer(1), 0.0);
    BookStat::ProcessCmdREAD(1, 1000);
    ASSERT_EQUAL(BookStat::Cheer(1), 1.0);
}
void TestMaxIDandMaxPage() {
    const int MAX_USER_COUNT = 100'000;
    BookStat::Init();
    ASSERT_EQUAL(BookStat::Cheer(MAX_USER_COUNT), 0.0);
    BookStat::ProcessCmdREAD(MAX_USER_COUNT, 1000);
    ASSERT_EQUAL(BookStat::Cheer(MAX_USER_COUNT), 1.0);
}
//    {// zero ID
//        ReadingManager manager;
//        ASSERT_EQUAL(manager.Cheer(0), 0.0);
//        manager.Read(0, 1000);
//        ASSERT_EQUAL(manager.Cheer(0), 1.0);
//    }

void Test() {
    RUN_TEST(TestAddUser);
    RUN_TEST(TestAddUserButCheerNotExisted);
    RUN_TEST(TestReadTwice);
    RUN_TEST(TestBetterThanThird);
    RUN_TEST(TestBetterThan2Third);
    RUN_TEST(TestMaxID);
    RUN_TEST(TestMaxPage);
    RUN_TEST(TestMaxIDandMaxPage);
    //RUN_TEST(TestMatchedDocumentsByStatus);
}





int main() {

    Test();


    int request_count;
    std::cin >> request_count >> std::ws;

    if (request_count > 0) {

        BookStat::Init();

        for (int i = 0; i < request_count; ++i) {
            CMD Cmd;
            // parse command
            std::cin >> Cmd.CommandType;
            std::cin >> Cmd.user_id;
            if (Cmd.CommandType == "READ"s) {
                std::cin >> Cmd.page_num;
            }
            // process command
            if (Cmd.CommandType == "CHEER"s) {
                BookStat::ProcessCmdCHEER(Cmd.user_id);
            }
            else if (Cmd.CommandType == "READ"s) {
                BookStat::ProcessCmdREAD(Cmd.user_id, Cmd.page_num);
            }
        }
    }
}