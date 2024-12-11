#include <iostream>
#include <sqlite_modern_cpp.h>
#include <tgbot/tgbot.h>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>

const std::string TOKEN{ "Token" }; 
using namespace sqlite;
using namespace std;
struct GeoPhoto {
    string photo1 = "https://travel-armenia.ru/wp-content/uploads/2022/08/flag_of_armenia.svg_.webp";
    string answer1 = u8"Армянский";
    string photo2 = "https://www.prlib.ru/sites/default/files/book_preview/97133656-9ca2-45c1-8648-53e93ba219fe/293661_doc1.jpg";
    string answer2 = u8"Республика Башкортостан";
    string photo3 = "https://premiumflag.ru/upload/static/990/1.jpg";
    string answer3 = u8"СССР";
};

class BotAssistant {
public:
    void run() {
        srand(static_cast<unsigned int>(time(0))); 
        check_txt();
    }

private:
    std::map<int64_t, std::string> correct_answers; 

    void check_txt() {
        TgBot::Bot bot(TOKEN);
        GeoPhoto geo;

        bot.getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
            bot.getApi().sendMessage(message->chat->id, u8"Привет. Введите /test, чтобы начать игру.");
            });

        bot.getEvents().onCommand("test", [&](TgBot::Message::Ptr message) {
            send_question(message, geo);
            });

        bot.getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query) {
            handle_answer(query);
            });

        try {
            TgBot::TgLongPoll longPoll(bot);
            while (true) {
                printf("Начался длинный опрос\n");
                longPoll.start();
            }
        }
        catch (TgBot::TgException& e) {
            printf("Error: %s\n", e.what());
        }
    }

    void send_question(TgBot::Message::Ptr message, GeoPhoto geo) {
        std::map<std::string, bool> answers;
        string true_answer;
        int x = rand() % 3; 
        switch (x) {
        case 0:
            true_answer = geo.answer1;
            answers = { {geo.answer1, true}, {geo.answer2, false}, {geo.answer3, false} };
            break;
        case 1:
            true_answer = geo.answer2;
            answers = { {geo.answer1, false}, {geo.answer2, true}, {geo.answer3, false} };
            break;
        case 2:
            true_answer = geo.answer3;
            answers = { {geo.answer1, false}, {geo.answer2, false}, {geo.answer3, true} };
            break;
        }

       
        correct_answers[message->chat->id] = true_answer;

        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        add_buttons(keyboard, answers); 
        TgBot::Bot bot(TOKEN); 
        bot.getApi().sendPhoto(message->chat->id, get_photo_path(x, geo));
        bot.getApi().sendMessage(message->chat->id, u8"Выберите правильный ответ:", false, 0, keyboard);
    }

    void handle_answer(TgBot::CallbackQuery::Ptr query) {
        TgBot::Bot bot(TOKEN);
        string user_answer = query->data;
        int64_t user_id = query->message->chat->id;

      
        if (correct_answers.find(user_id) != correct_answers.end()) {
            if (user_answer == correct_answers[user_id]) {
                bot.getApi().sendMessage(user_id, u8"Правильно!");
            }
            else {
                bot.getApi().sendMessage(user_id, u8"Неверно! Правильным ответом было:" + correct_answers[user_id]);
            }
        }
    }

    void add_buttons(TgBot::InlineKeyboardMarkup::Ptr& keyboard, const std::map<std::string, bool>& answers) {
        for (const auto& answer : answers) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text = answer.first;
            button->callbackData = answer.first; 
            keyboard->inlineKeyboard.push_back({ button });
        }
    }

    string get_photo_path(int index, GeoPhoto geo) {
        switch (index) {
        case 0: return geo.photo1;
        case 1: return geo.photo2;
        case 2: return geo.photo3;
        default: return geo.photo1; 
        }
    }
};

int main() {
    BotAssistant b;
    b.run();
    return 0;
}