#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"
#define RESET   "\x1b[0m"

#define BLACK_BG   "\x1b[40m"
#define RED_BG     "\x1b[41m"
#define GREEN_BG   "\x1b[42m"
#define YELLOW_BG  "\x1b[43m"
#define BLUE_BG    "\x1b[44m"
#define MAGENTA_BG "\x1b[45m"
#define CYAN_BG    "\x1b[46m"
#define WHITE_BG   "\x1b[47m"

typedef struct Card Card;

struct Card {
    uint8_t rank_suit;
    Card* next;
};

typedef struct Player Player;

struct Player {
    uint32_t cash;
    uint8_t score;
    uint8_t alt_score;
    Card* hand;
    Card* hidden_hand;
};

typedef struct Deck Deck;

struct Deck {
    uint8_t size;
    Card* head;
};

void clearscreen();
void card_ascii(Card* card, uint8_t card_num, char* card_string);
void clearscreen();
void displayMenu();
Card* destruct_cardlist(Card *cardlist);
void fill_deck(Deck *deck);
Card* pop_card(Deck *deck, uint8_t card_num);
Card* push_back_card(Card* hand, Card* card);
void game();
void initial_phase(Player* player, Player* dealer, Deck* deck);
int8_t betting_phase(Player* player, Player* dealer);
uint8_t black_jack_check(Player* player, Player* dealer);
uint8_t hit_or_stand(Player* player, Deck* deck);
void dealer_draw(Player* player, Player* dealer, Deck* deck);
uint8_t check_player_busted(Player* player, Player* dealer);
void check_dealer_busted(Player* player, Player* dealer);
void take_card_and_calculate_score(Player* player, Deck* deck);
void print_table_hidden(Player* player, Player* dealer);
void print_table_visible(Player* player, Player* dealer);
void hide_dealer_cards(Player* dealer);

int main() {
    uint8_t choice;
    srand(time(NULL));

    while (1) {
        clearscreen();
        displayMenu();
        scanf("%hhu", &choice);

        switch (choice) {
            case 1:
                printf("Starting game...\n");
                game();
                break;
            case 2:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
        
        
    }

    return 0;
}

void clearscreen()
{
    printf("\033[1;1H\033[2J");
}

void card_ascii(Card* cardlist, uint8_t card_num, char* card_string)
{
    uint8_t i, suit;
    char card_line[10][256];
    Card* card = cardlist;

    for(i = 0; i < sizeof(card_line)/sizeof(card_line[0]); i++)
        card_line[i][0] = '\0';

    for(i = 0; i < card_num; i++)
    {
        char suit_char[13] = "";
        char rank_char[14] = "";

        suit = card->rank_suit & 0x0f;
        
        switch (suit) {
            case 1:
                strcpy(suit_char, RED "♥" RESET) ; // Hearts
                strcpy(rank_char, RED);
                break;
            case 2:
                strcpy(suit_char, RESET "♣" RESET); // Clubs
                strcpy(rank_char, RESET);
                break;
            case 4:
                strcpy(suit_char, RED "♦" RESET); // Diamonds
                strcpy(rank_char, RED);
                break;
            case 8:
                strcpy(suit_char, RESET "♠" RESET); // Spades
                strcpy(rank_char, RESET);
                break;
            default:
                strcpy(suit_char, RESET "?" RESET); // Unknown
                strcpy(rank_char, RESET);
        }

        switch (card->rank_suit >> 4) {
            case 1:
                strcat(rank_char, " A");
                break;
            case 2:
                strcat(rank_char, " 2");
                break;
            case 3:
                strcat(rank_char, " 3");
                break;
            case 4:
                strcat(rank_char, " 4");
                break;
            case 5:
                strcat(rank_char, " 5");
                break;
            case 6:
                strcat(rank_char, " 6");
                break;
            case 7:
                strcat(rank_char, " 7");
                break;
            case 8:
                strcat(rank_char, " 8");
                break;
            case 9:
                strcat(rank_char, " 9");
                break;
            case 10:
                strcat(rank_char, "10");
                break;
            case 11:
                strcat(rank_char, " J");
                break;
            case 12:
                strcat(rank_char, " Q");
                break;
            case 13:
                strcat(rank_char, " K");
                break;
            default:
                strcat(rank_char, "  ");
        }

        strcat(rank_char, RESET);

        char aux_string[256];

        strcat(card_line[0], "  __________  ");
        sprintf(aux_string, " /%s        \\ ", rank_char);
        strcat(card_line[1], aux_string);
        switch(suit)
        {
            case 1:
                strcat(card_line[2], "|" RED "   __  __   " RESET "|");
                strcat(card_line[3], "|" RED "  /  \\/  \\  " RESET "|");
                strcat(card_line[4], "|" RED " |        | " RESET "|");
                strcat(card_line[5], "|" RED "  \\      /  " RESET "|");
                strcat(card_line[6], "|" RED "   \\    /   " RESET "|");
                strcat(card_line[7], "|" RED "    \\  /    " RESET "|");
                strcat(card_line[8], "|" RED "     \\/     " RESET "|");
                break;
            case 2:
                strcat(card_line[2], "|" RESET "    ____    " RESET "|");
                strcat(card_line[3], "|" RESET "   /    \\   " RESET "|");
                strcat(card_line[4], "|" RESET "  _\\    /_  " RESET "|");
                strcat(card_line[5], "|" RESET " /        \\ " RESET "|");
                strcat(card_line[6], "|" RESET " \\__/||\\__/ " RESET "|");
                strcat(card_line[7], "|" RESET "     ||     " RESET "|");
                strcat(card_line[8], "|" RESET "     ||     " RESET "|");
                break;
            case 4:
                strcat(card_line[2], "|" RED "     /\\     " RESET "|");
                strcat(card_line[3], "|" RED "    /  \\    " RESET "|");
                strcat(card_line[4], "|" RED "   /    \\   " RESET "|");
                strcat(card_line[5], "|" RED "  .      .  " RESET "|");
                strcat(card_line[6], "|" RED "   \\    /   " RESET "|");
                strcat(card_line[7], "|" RED "    \\  /    " RESET "|");
                strcat(card_line[8], "|" RED "     \\/     " RESET "|");
                break;
            case 8:
                strcat(card_line[2], "|" RESET "     /\\     " RESET "|");
                strcat(card_line[3], "|" RESET "    /  \\    " RESET "|");
                strcat(card_line[4], "|" RESET "   /    \\   " RESET "|");
                strcat(card_line[5], "|" RESET "  /      \\  " RESET "|");
                strcat(card_line[6], "|" RESET " |   __   | " RESET "|");
                strcat(card_line[7], "|" RESET "  \\_//\\\\_/  " RESET "|");
                strcat(card_line[8], "|" RESET "    /__\\    " RESET "|");
                break;
            default:
                strcat(card_line[2], "|" GREEN " ########## " RESET "|");
                strcat(card_line[3], "|" GREEN " ########## " RESET "|");
                strcat(card_line[4], "|" GREEN " ########## " RESET "|");
                strcat(card_line[5], "|" GREEN " ########## " RESET "|");
                strcat(card_line[6], "|" GREEN " ########## " RESET "|");
                strcat(card_line[7], "|" GREEN " ########## " RESET "|");
                strcat(card_line[8], "|" GREEN " ########## " RESET "|");
        }
        sprintf(aux_string, " \\________%s/ ", rank_char);        
        strcat(card_line[9], aux_string);
        if(card->next == NULL)
            break;
        card = card->next;
    }
    
    for(i = 0; i < sizeof(card_line)/sizeof(card_line[0]); i++)
        strcat(card_line[i], "\n");

    for(i = 0; i < sizeof(card_line)/sizeof(card_line[0]); i++)
        strcat(card_string, card_line[i]);
}

void displayMenu() {

    printf(GREEN_BG BLACK "▗▄▄▖ ▗▖    ▗▄▖  ▗▄▄▖▗▖ ▗▖" WHITE "   ▗▖ ▗▄▖  ▗▄▄▖▗▖ ▗▖\n" RESET);
    printf(GREEN_BG BLACK "▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌   ▐▌▗▞▘" WHITE "   ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘\n" RESET);
    printf(GREEN_BG BLACK "▐▛▀▚▖▐▌   ▐▛▀▜▌▐▌   ▐▛▚▖ " WHITE "   ▐▌▐▛▀▜▌▐▌   ▐▛▚▖ \n" RESET);
    printf(GREEN_BG BLACK "▐▙▄▞▘▐▙▄▄▖▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌" WHITE "▗▄▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌\n" RESET);
    printf(GREEN_BG BLACK "                                             \n" RESET);
                                             
    printf(YELLOW "1. Start Game\n" RESET);
    printf(RED "2. Exit\n" RESET);
    printf(BLUE "Enter your choice: " RESET);
}

Card* destruct_cardlist(Card *cardlist)
{
    if(cardlist == NULL)
        return NULL;
    if(cardlist->next != NULL)
        cardlist->next = destruct_cardlist(cardlist->next);
    free(cardlist);
    cardlist = NULL;
    return NULL;
}

void fill_deck(Deck *deck)
{
    uint8_t i, j;
    Card* card;
    card = (Card*)malloc(sizeof(Card));
    assert(card != NULL);
    deck->head = card;
    deck->size = 0;

    for(i = 1; i < 9; i *= 2)
    {
        for(j = 1; j <= 13; j++)
        {
            card->rank_suit = (j << 4) + i;
            deck->size++;
            if(i == 8 && j == 13)
                card->next = NULL;
            else
            {
                card->next = (Card*)malloc(sizeof(Card));
                assert(card != NULL);
                card = card->next;
            }
        }
    }
    return;
}

Card* pop_card(Deck *deck, uint8_t card_num)
{
    uint8_t i;
    Card* card;
    Card* prev_card = deck->head;
    deck->size--;
    if(card_num == 0)
    {
        card = deck->head;
        deck->head = card->next;
        card->next = NULL;
        return card;
    }
    for(i = 0; i < card_num - 1; i++)
        prev_card = prev_card->next;
    card = prev_card->next;
    prev_card->next = card->next;
    card->next = NULL;
    return card;
}

Card* push_back_card(Card* hand, Card* card)
{
    Card* prev_card = hand;
    if(hand == NULL)
        return card;
    while(prev_card->next != NULL)
        prev_card = prev_card->next;
    prev_card->next = card;
    return hand;
}

void hide_dealer_cards(Player* dealer)
{
    Card* card_visible;
    Card* card_hidden;;
    dealer->hidden_hand = (Card*)malloc(sizeof(Card));
    assert(dealer->hidden_hand != NULL);
    dealer->hidden_hand->next = NULL;
    card_visible = dealer->hand;
    card_hidden = dealer->hidden_hand;
    card_hidden->rank_suit = card_visible->rank_suit;
    card_visible = card_visible->next;
    while(card_visible != NULL)
    {
        card_hidden->next = (Card*)malloc(sizeof(Card));
        assert(card_hidden->next != NULL);
        card_hidden->next->next = NULL;
        card_hidden = card_hidden->next;
        card_hidden->rank_suit = 0;
        card_visible = card_visible->next;
    }
}

void print_table_hidden(Player* player, Player* dealer)
{
    char player_string[2560] = "";
    char dealer_string[2560] = "";
    clearscreen();
    card_ascii(player->hand, 12, player_string);
    hide_dealer_cards(dealer);
    card_ascii(dealer->hidden_hand, 12, dealer_string);
    printf("\tDealer's hand: Score: ?\n%s\n", dealer_string);
    printf("\tPot: %hu\n\n", dealer->cash);
    printf("\tPlayer's cash: %hu\n", player->cash);
    if(player->alt_score == 0)
        printf("\tPlayer's hand: Score: %hu\n%s\n", player->score, player_string);
    else
        printf("\tPlayer's hand: Score: %hu (%hu)\n%s\n", player->score, player->alt_score, player_string);
}

void print_table_visible(Player* player, Player* dealer)
{
    char player_string[2560] = "";
    char dealer_string[2560] = "";
    clearscreen();
    card_ascii(player->hand, 12, player_string);
    card_ascii(dealer->hand, 12, dealer_string);
    printf("\tDealer's hand: Score: %hu\n%s\n", dealer->score, dealer_string);
    printf("\tPot: %hu\n\n", dealer->cash);
    printf("\tPlayer's cash: %hu\n", player->cash);
    printf("\tPlayer's hand: Score: %hu\n%s\n", player->score, player_string);
}

void game()
{
    Player player, dealer;
    Deck deck;
    uint8_t game_over = 0;
    uint8_t game_over_reason = 0;
    uint8_t choice;
    deck.size = 0;
    deck.head = NULL;
    player.hand = NULL;
    player.hidden_hand = NULL;
    player.cash = 1000;
    dealer.cash = 0;
    dealer.hand = NULL;
    dealer.hidden_hand = NULL;

    while(!game_over)
    {
        clearscreen();
        if(betting_phase(&player, &dealer) == -1)
        {
            game_over = 1;
            game_over_reason = 1;
            break;
        }
        
        initial_phase(&player, &dealer, &deck);
        if(black_jack_check(&player, &dealer))
        {
            printf("Your cash: %hu\n", player.cash);
            printf("Continue playing? (1 - Yes, 0 - No): ");
            scanf("%hhu", &choice);
            game_over = !choice;
            continue;
        }
        while(player.score < 21)
        {
            if(hit_or_stand(&player, &deck))
            {
                print_table_hidden(&player, &dealer);
                if(black_jack_check(&player, &dealer))
                    break;
            }
            else
                break;
        }
        if(player.score == 21)
        {
            printf("Your cash: %hu\n", player.cash);
            printf("Continue playing? (1 - Yes, 0 - No): ");
            scanf("%hhu", &choice);
            game_over = !choice;
            continue;
        }
        if(check_player_busted(&player, &dealer))
        {
            printf("Your cash: %hu\n", player.cash);
            printf("Continue playing? (1 - Yes, 0 - No): ");
            scanf("%hhu", &choice);
            game_over = !choice;
            continue;
        }

        dealer_draw(&player, &dealer, &deck);
        printf("Your cash: %hu\n", player.cash);
        printf("Continue playing? (1 - Yes, 0 - No): ");
        scanf("%hhu", &choice);
        game_over = !choice;
        continue;
    }
    clearscreen();
    switch (game_over_reason)
    {
    case 0:
        printf("You've got %hu cash. Thanks for playing!\n", player.cash);
        break;

    case 1:
        printf("You've got %hu cash, not enough to bet. Thanks for playing!\n", player.cash);
        break;
    
    default:
        break;
    }
    printf("Press any key to continue...\n");
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
    
    deck.head = destruct_cardlist(deck.head);
    player.hand = destruct_cardlist(player.hand);
    player.hidden_hand = destruct_cardlist(player.hidden_hand);
    dealer.hand = destruct_cardlist(dealer.hand);
    dealer.hidden_hand = destruct_cardlist(dealer.hidden_hand);
}

void initial_phase(Player* player, Player* dealer, Deck* deck)
{
    uint8_t i;
    player->hand = destruct_cardlist(player->hand);
    player->hidden_hand = destruct_cardlist(player->hidden_hand);
    dealer->hand = destruct_cardlist(dealer->hand);
    dealer->hidden_hand = destruct_cardlist(dealer->hidden_hand);
    deck->head = destruct_cardlist(deck->head);
    if(deck != NULL)
    {
        deck->size = 0;
        deck->head = NULL;
    }
    fill_deck(deck);
    for(i = 0; i < 2; i++)
    {
        take_card_and_calculate_score(player, deck);
        take_card_and_calculate_score(dealer, deck);
    }
    print_table_hidden(player, dealer);
    return;
}

int8_t betting_phase(Player* player, Player* dealer)
{
    uint16_t bet;
    printf("Your cash: %hu\n", player->cash);
    if(player->cash < 10 && dealer->cash == 0)
        return -1;
    printf("Enter your bet: ");
    scanf("%hu", &bet);
    while(((bet > player->cash || bet < 10) && dealer->cash == 0) || bet % 10 != 0)
    {
        if (bet > player->cash)
            printf("You don't have enough cash. Please enter a valid bet: ");
        else if(bet < 10)
            printf("Minimum bet is 10. Please enter a valid bet: ");
        else if(bet % 10 != 0)
            printf("The bet must be a multiple of 10. Please enter a valid bet: ");
        scanf("%hu", &bet);
    }
    player->cash -= bet;
    dealer->cash += bet;
    return 0;
}

uint8_t black_jack_check(Player* player, Player* dealer)
{
    if(player->score == 21)
    {
        if(dealer->score == 21)
        {
            print_table_visible(player, dealer);
            printf("You both have got Black Jack! It's a tie!\n");
            return 1;
        }
        print_table_visible(player, dealer);
        printf("You've got Black Jack! You win!\n");
        player->cash += dealer->cash * 5 / 2;
        dealer->cash = 0;
        return 1;
    }
    return 0;
}

uint8_t hit_or_stand(Player* player, Deck* deck)
{
    uint8_t choice;
    printf("Do you want to hit or stand? (1 - Hit, 2 - Stand): ");
    scanf("%hhu", &choice);
    if(choice == 1)
    {
        take_card_and_calculate_score(player, deck);
        return 1;
    }
    return 0;
}

void dealer_draw(Player* player, Player* dealer, Deck* deck)
{
    while(dealer->score < 17)
    {
        take_card_and_calculate_score(dealer, deck);
    }
    check_dealer_busted(player, dealer);
}

uint8_t check_player_busted(Player* player, Player* dealer)
{
    if(player->score > 21)
    {
        print_table_visible(player, dealer);
        printf("You've busted! You lose!\n");
        dealer->cash = 0;
        return 1;
    }
    return 0;
}

void check_dealer_busted(Player* player, Player* dealer)
{
    print_table_visible(player, dealer);
    if(dealer->score > 21)
    {
        printf("Dealer has busted! You win!\n");
        player->cash += dealer->cash * 2;
        dealer->cash = 0;
        return;
    }
    if(dealer->score > player->score)
    {
        printf("Dealer has won! You lose!\n");
        dealer->cash = 0;
        return;
    }
    if(dealer->score == player->score)
    {
        printf("It's a tie!\n");
        return;
    }
    if(dealer->score < player->score)
    {
        printf("You've won!\n");
        player->cash += dealer->cash * 2;
        dealer->cash = 0;
        return;
    }
}

void take_card_and_calculate_score(Player* player, Deck* deck)
{
    player->hand = push_back_card(player->hand, pop_card(deck, rand() % deck->size));
    uint8_t player_score = 0, alt_score = 0;
    Card* card = player->hand;
    while(card != NULL)
    {
        player_score += (card->rank_suit >> 4 ) > 10 ? 10 : (card->rank_suit >> 4);
        card = card->next;
    }
    card = player->hand;
    while(card != NULL)
    {
        if((card->rank_suit >> 4) == 1)
        {
            if(player_score + 10 <= 21)
            {
                alt_score = player_score;
                player_score += 10;
            }
            else
                alt_score = player_score + 10;
        }
        card = card->next;
    }
    player->score = player_score;
    player->alt_score = alt_score;
}