#include <catch2/catch.hpp>

#include <element/token.h>
#include "../src/token_internal.hpp"

//STD
#include <array>

void print_token(const element_tokeniser_ctx* context, const element_token* nearest_token)
{
    std::array<char, 2048> output_buffer{};
    element_tokens_to_string(context, nearest_token, output_buffer.data(), output_buffer.size());
    printf("%s", output_buffer.data());
    UNSCOPED_INFO(output_buffer.data());
}

TEST_CASE("Token Generation", "[Tokeniser]") {

    element_tokeniser_ctx* tokeniser;
    element_tokeniser_create(&tokeniser);

    SECTION("Number") {
        // A number has to be 'closed' with semicolon to be tokenised as a number
        element_tokeniser_run(tokeniser, "9;", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_NUMBER);
    }

    SECTION("Identifier") {
        element_tokeniser_run(tokeniser, "val", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_IDENTIFIER);
    }

    SECTION("Underscore") {
        // A underscore has to be 'closed' with a semicolon to be tokenised as an underscore
        element_tokeniser_run(tokeniser, "_;", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_UNDERSCORE);
    }

    SECTION("Dot") {
        element_tokeniser_run(tokeniser, ".", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_DOT);
    }

    SECTION("Left Bracket") {
        element_tokeniser_run(tokeniser, "(", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_BRACKETL);
    }

    SECTION("Right Bracket") {
        element_tokeniser_run(tokeniser, ")", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_BRACKETR);
    }

    SECTION("Semicolon") {
        element_tokeniser_run(tokeniser, ";", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_SEMICOLON);
    }

    SECTION("Colon") {
        element_tokeniser_run(tokeniser, ":", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_COLON);
    }

    SECTION("Comma") {
        element_tokeniser_run(tokeniser, ",", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_COMMA);
    }

    SECTION("Left Brace") {
        element_tokeniser_run(tokeniser, "{", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_BRACEL);
    }

    SECTION("Right Brace") {
        element_tokeniser_run(tokeniser, "}", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_BRACER);
    }

    SECTION("Equals") {
        element_tokeniser_run(tokeniser, "=", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_EQUALS);
    }

    SECTION("Everything") {
        element_tokeniser_run(tokeniser, "v.():,{}=9_;", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_IDENTIFIER); // v
        REQUIRE(tokeniser->tokens[1].type == ELEMENT_TOK_DOT); // .
        REQUIRE(tokeniser->tokens[2].type == ELEMENT_TOK_BRACKETL); // (
        REQUIRE(tokeniser->tokens[3].type == ELEMENT_TOK_BRACKETR); // )
        REQUIRE(tokeniser->tokens[4].type == ELEMENT_TOK_COLON); // :
        REQUIRE(tokeniser->tokens[5].type == ELEMENT_TOK_COMMA); // ,
        REQUIRE(tokeniser->tokens[6].type == ELEMENT_TOK_BRACEL); // {
        REQUIRE(tokeniser->tokens[7].type == ELEMENT_TOK_BRACER); // }
        REQUIRE(tokeniser->tokens[8].type == ELEMENT_TOK_EQUALS); // =
        REQUIRE(tokeniser->tokens[9].type == ELEMENT_TOK_NUMBER); // 9
        REQUIRE(tokeniser->tokens[10].type == ELEMENT_TOK_UNDERSCORE); // _
        REQUIRE(tokeniser->tokens[11].type == ELEMENT_TOK_SEMICOLON); // ;
    }

    SECTION("Example Code") {
        element_tokeniser_run(tokeniser, "evaluate = Num.add(1, 2);", "<input>");
        print_token(tokeniser, nullptr);
        REQUIRE(tokeniser->tokens[0].type == ELEMENT_TOK_IDENTIFIER);
        REQUIRE(tokeniser->tokens[1].type == ELEMENT_TOK_EQUALS);
        REQUIRE(tokeniser->tokens[2].type == ELEMENT_TOK_IDENTIFIER);
        REQUIRE(tokeniser->tokens[3].type == ELEMENT_TOK_DOT);
        REQUIRE(tokeniser->tokens[4].type == ELEMENT_TOK_IDENTIFIER);
        REQUIRE(tokeniser->tokens[5].type == ELEMENT_TOK_BRACKETL);
        REQUIRE(tokeniser->tokens[6].type == ELEMENT_TOK_NUMBER);
        REQUIRE(tokeniser->tokens[7].type == ELEMENT_TOK_COMMA);
        REQUIRE(tokeniser->tokens[8].type == ELEMENT_TOK_NUMBER);
        REQUIRE(tokeniser->tokens[9].type == ELEMENT_TOK_BRACKETR);
        REQUIRE(tokeniser->tokens[10].type == ELEMENT_TOK_SEMICOLON);
    }
}