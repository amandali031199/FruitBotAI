// fruit_bot.c
// Assignment 3, COMP1511 18s1: Fruit Bot
//
// This program by YOUR-NAME-HERE (z5555555) on INSERT-DATE-HERE
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: minor bug in main fixed

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fruit_bot.h"

#define TRUE 0
#define FALSE 1
#define LOW_BAT 10
#define NOT_COMPLETE 1
#define COMPLETE 0
#define MAX_PRICE -10000000

void print_player_name(void);
void print_move(struct bot *b);
void run_unit_tests(void);

// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE
int battery_low(struct bot *b);
int fruit_left(struct bot *b);
int n_locations(struct bot *b);
int can_buy(struct bot *b, struct location *l);
int can_sell(struct bot *b, struct location *l);
int have_space(struct bot *b);
int shortest_distance(struct location *l1, struct location *l2);
int location_sells_fruit(struct location *l);
int exists_location_buys_same_fruit(struct location *l);
struct location *location_buys_same_fruit(struct location *l);
int nearest_electricity(struct bot *b);
int location_has_elec(struct location *l);
struct location *cheapest_electricity(struct bot *b);
void find_max_profit(struct bot *b, struct location **seller,
					 struct location **buyer);
int sells_unique_fruit(struct location *curr,struct bot *b);
struct location *find_cheapest_seller(struct location *curr, struct bot *b);
struct location *find_cheapest_seller_large(struct location *curr,
										    struct bot *b);
struct location *find_dearest_buyer(struct location *curr, struct bot *b, 
									struct location *seller);
struct location *find_dearest_buyer_large(struct location *curr, struct bot *b,
										  struct location *seller);
struct location *find_dearest_buyer_no_fruit(struct location *curr, 
										     struct bot *b, 
											 struct location *seller);
struct location *find_dearest_buyer_large_no_fruit(struct location *curr, 
												   struct bot *b,
										 		   struct location *seller);
int location_buys_fruit(struct location *l);
struct location *match_name_to_location(struct bot *b, char *name);
int n_bots_at_location(struct location *l, struct bot *b);
int n_bots_at_location_with_fruit(struct location *l, struct bot *b);
void compare_profits(int i, int profit[], char *seller_name[], 
					 char *buyer_name[], struct location **seller, 
					 struct location **buyer, struct bot *b);
struct location *find_dearest_buyer_given_fruit(struct bot *b);
struct location *find_dearest_buyer_given_fruit_large(struct bot *b,int amount);
void not_carrying_fruit(struct bot *b);
int at_can_buy_location(struct bot *b,int energy_required);
int last_resort(struct bot *b);
int at_elec_carrying_fruit(struct bot *b);
int electricity(struct bot *b);
void decide_moves(struct bot *b, struct location *move_to_location,int move,
				  int energy_required, struct location *future_location);
int small_world(struct bot *b,int first_iteration, struct location *curr, 
				char *seller_name[], char *buyer_name[],int i);
int large_world(struct bot *b, struct location *curr, char *seller_name[],
				char *buyer_name[], int i);
void fill_profit(int j, int i, struct bot *b, char *seller_name[],
				 char *buyer_name[],int profit[]);
struct location *scan_world_east(struct bot *b, struct location *compare,
								struct location *curr,int first_iteration,
								int n_moves,int n_bots,int cheapest_price);
int first_fruit(struct bot *b,int first_iteration,struct location *loop,
				char *seller_name[], char *buyer_name[],
				struct location **seller,struct location **buyer,int profit[]);


// YOU SHOULD NOT NEED TO CHANGE THIS MAIN FUNCTION

int main(int argc, char *argv[]) {

    if (argc > 1) {
        // supply any command-line argument to run unit tests
        run_unit_tests();
        return 0;
    }

    struct bot *me = fruit_bot_input(stdin);
    if (me == NULL) {
        print_player_name();
    } else {
        print_move(me);
    }

    return 0;
}

void print_player_name(void) {
    // CHANGE THIS PRINTF TO YOUR DESIRED PLAYER NAME
    printf("Jungshook");
}


void print_move(struct bot *b) {
	assert(b != NULL);
	assert(b->location != NULL);
	int move = NOT_COMPLETE;
	int amount = 0;
	int n_loc = n_locations(b);
	int energy_required = 0;
	struct location *move_to_location = NULL;
	struct location *future_location = NULL;

	move = electricity(b);

	if (strcmp(b->location->fruit,"Electricity") == TRUE && 
		battery_low(b) != TRUE && b->fruit != NULL){ 
			//if finished refilling resume finding buyer
			move = at_elec_carrying_fruit(b);
	} if (b->fruit != NULL && move == NOT_COMPLETE){ //carrying fruit 
		//printf("Within carrying fruit");
		if (can_buy(b,b->location) == TRUE){ //at seller location
			move = at_can_buy_location(b,energy_required);
		} else if (can_sell(b,b->location) == TRUE){ //at  buyer location
		//printf("Within at can sell location\n");
			amount = b->location->quantity;
			assert(amount != 0);
			printf("Sell %d\n",amount);
			move = COMPLETE;
		} else {
			move = last_resort(b);
		}

	} else if (b->fruit == NULL && move == NOT_COMPLETE){ //not carrying fruit
		not_carrying_fruit(b);
	}
}


// ADD A COMMENT HERE EXPLAINING YOUR OVERALL TESTING STRATEGY

void run_unit_tests(void) {
    // PUT YOUR UNIT TESTS HERE
    // All of my functions take in struct bot or struct location, therefore
	// they are not 'testable' independently without creating worlds/structs. 
	
	//DESCRIPTION OF TESTING STRATEGY
	//I ran my code through a variety of worlds (small,medium, large and a 2048
	//location world) and both single & multi-bot worlds to make sure it works
	// for all cases.
	//ASSERTS:
	// I put asserts inside my functions e.g. assert(location != NULL) 
	// or assert(n_bots != 0) to make sure I'm not dividing numbers by 0, and 
	//that when i type location->fruit, it doesn't return an error. 
	//These were used to also test the smaller functions.
	//By testing the larger strategy path functions, I was also testing the
	//smaller functions they called.

	//PRINTF: 
	//To help debug I placed printf's inside if statements and functions to make
	//sure my program follows the correct logic path. It also helped me debug 	
	//errors. For example i would run the referee and manually find the fruit
	//with highest profit and see if the bot does what I expect it to do

	//I used different worlds to test all these following cases: 
	
	//AT ELEC LOCATION
		//BATTERY LOW
			//LOCATION HAS BATTERY TO SELL	
				//electricity->buy
			//LOCATION DOESNT HAVE BATTERY TO SELL 
				//electricity->move 
		//BATTERY NOT LOW
			//CARRYING FRUIT
				//within at elec carrying fruit->move
			//NOT CARRYING FRUIT 	
				//SURROUNDING AREA SOLD OUT
					//within not carrying fruit->sold out->move
				//SURROUNDING AREA NOT SOLD OUT
					//ENOUGH BATTERY
						//ENOUGH TURNS LEFT
							//within not carrying fruit->within decide moves->		
							//move
						//NOT ENOUGH TURNS LEFT
							//within not carrying fruit->within decide moves->
							//move 0
					//NOT ENOUGH BATTERY
						//ENOUGH TURNS LEFT
							//within not carrying fruit->with decide moves->
							//move
						//NOT ENOUGH TURNS LEFT
							///within not carrying fruit->within decide moves->
							//move 0
				
	//NOT AT ELEC LOCATION 
		//BATTERY LOW
			//electricity->move
		//BATTERY NOT LOW 
			//CARRYING FRUIT
				//AT SELLER
					//within carrying fruit->within at can buy location->move
				//AT BUYER
					//within carrying fruit->within at can sell location->sell
				//OTHER
					//within carryin gfruit->within last resort->move
			//NOT CARRYING FRUIT 	
				//SURROUNDING AREA SOLD OUT
					//within not carrying fruit->sold out->move
				//SURROUNDING AREA NOT SOLD OUT
					//ENOUGH BATTERY
						//ENOUGH TURNS LEFT
							//within not carrying fruit->within decide moves->		
							//move
						//NOT ENOUGH TURNS LEFT
							//within not carrying fruit->within decide moves->
							//move 0
					//NOT ENOUGH BATTERY
						//ENOUGH TURNS LEFT
							//within not carrying fruit->with decide moves->
							//move
						//NOT ENOUGH TURNS LEFT
							///within not carrying fruit->within decide moves->
							//move 0

	//This can be seen used in the following examples
	
	//EXAMPLE 1: SMALL SINGLE WORLD: using small world strategy paths
	//WORLD DETAILS 
	//battery_capacity = 35
	//maximum_fruit_kg = 32
	//maximum_move = 9
	
	//CSE:other
	//Quadrangle: other
	//Campus Compost Heap: will buy 1603kg of Anything for $1/kg
	//Campus Charging: will sell 16kJ of Electricity for $3/kJ
	//CLB 7: will buy 49kg of Apples for $34/kg
	//Kensington Apple Farm: will sell 130kg of Apples for $10/kg
	//Matthews A: will buy 22kg of Apples for $31/kg

	//BOT is at CSE with $138, battery level:35 turn_left:12
	//PATH: (seller: Kensington Apple Farm, buyer:CLB 7) 
	//within not carrying fruit -> within decide moves -> Move to Kensington

	//BOT is at Kensington Apple Farm with $138,battery level: 33 turn_left:11
	//PATH: (seller: Kensington Apple Farm, buyer:CLB 7)
	//within not carrying fruit->with decide moves->Buy 49

	//Kensington Apple Farm: will sell 117kg of Apples for $10/kg
	
	//BOT is at Kensington Apple Farm with $8, battery level:33 turn_left:10
	//13 kg of Apples
	//PATH: (seller: Kensington Apple Farm, buyer:CLB 7)
	//within carrying fruit->within at can buy location->move to CLB 7

	//BOT is at CLB 7 with $8, battery level:32 turn_left:9,13kg of Apples
	//PATH: (seller: Kensington Apple Farm, buyer:CLB 7)
	//within carrying fruit->within at can sell location->Sell 49

	//CLB 7: will buy 36 Apples for $34/kg

	//BOT is at CLB 7 with $450 battery level:32 turn_left:8
	//PATH:(seller: Kensington Apple Farm, buyer:CLB 7)
	//within not carrying fruit -> within decide moves -> Move to Kensington

	//BOT is at CLB 7 with $450 battery level: 2 turn_left:3
	//PATH:(seller: Kensington Apple Farm, buyer:CLB 7) battery & turns_left low
	//within not carrying fruit->with decide moves->move 0

	//BOT is at CLB 7 with $450 battery level: 6 turn_left:3
	//PATH:(seller: Kensington Apple Farm, buyer:CLB 7)  turns_left low
	//within not carrying fruit->with decide moves->move 0

	//EXAMPLE 2: SMALL WORLD: nothing left to buy 
	//WORLD DETAILS: 
	//CSE:other
	//Campus Compost Heap: will buy 1411kg of Anything for $1/kg
	//Matthews A: will buy 0kg of Apples for $21/kg
	//Quadrangle: other
	//Campus Charging: will sell 52kJ of Electricity for $6/kJ
	//CLB 7: will buy 0kg of Apples for $15/kg
	//Kensington Apple Farm: will sell 8kg of Apples for $12/kg

	//BOT is at CLB 7 with $402 battery level:15 turns_left = 4
	//PATH: no seller no buyer 
	//within not carrying fruit ->sold out->move (max_move)

	//In Small Worlds, I can't test cases which involve buying electricity, 
	//because there's no need to. That's why I also test Medium and Large Worlds.
	//Following Examples are Medium Worlds because Large worlds have too many	
	//locations to copy here. 

	//EXAMPLE 3: MEDIUM WORLD: Dealing with Electricity 
	//WORLD DETAILS: 
	//battery_capacity=62
	//maximum_fruit_kg = 14
	//maximum_move = 9

	//John Lions Garden: other
	//Anzac Parade: other
	//Kingsford Oranges: will sell 38 kg of Oranges for $36/kg
	//Maroubra Melons: will sell 38 kg of Watermelons for $7/kg
	//Eastgardens Fruit: will sell 35 kg of Kiwifruit for $10/kg
	//Village Green: other
	//Kensington Kiwifruit: will sell 52 kg of Kiwifruit for $12/kg
	//UNSW Green Waste: will buy 1344 kg of Anything for $1/kg
	//Mathews B: will buy 42 kg of Watermelons for $11/kg
	//Viola Lab: will buy 31 kg of Durian for $160/kg
	//Discount Electrons: will sell 0 kJ of Electricity for $5/kJ
	//K17 Basement: will buy 48 kg of Bananas for $56/kg
	//Science Theatre: will buy 41 kg of Kiwifruit for $23/kg
	//Power For You: will sell 47 kJ of Electricity for $4/kJ
	//Smelly Fruits R Us: will sell 36 kg of Durian for $110/kg
	//Campus Citrus Centre: will sell 32 kg of Oranges for $46/kg
	//Physics Lawn: other
	//Rosebery Orange Grove: will sell 53 kg of Oranges for $56/kg
	//Kora Lab: will buy 63 kg of Durian for $182/kg
	//CLB 8: will buy 33 kg of Kiwifruit for $20/kg
	//Sitar Lab: will buy 51 kg of Durian for $147/kg
	//Botany Produce: will sell 42 kg of Kiwifruit for $11/kg
	//Wentworth Watermelons: will sell 50 kg of Watermelons for $4/kg
	//Clancy Auditorium: will buy 31 kg of Oranges for $84/kg
	//J17 G03: will buy 59 kg of Oranges for $91/kg
	//Mathews C: will buy 15 kg of Kiwifruit for $24/kg
	//Clovelly Watermelons: will sell 40 kg of Watermelons for $8/kg
	//Physics Theatre: will buy 34 kg of Bananas for $34/kg
	//Paddington Banana Plantation: will sell 39 kg of Bananas for $17/kg
	//Bondi Banana Growers: will sell 44 kg of Bananas for $18/kg
	//Quadrangle Lawn: other

	//TESTING BATTERY_LOW_FUNCTION
	//BOT is at 'Power For You', battery level: 29 turns_left: 40,10kg Bananas
	//PATH: battery low,location has battery to sell
	//electricity->buy

	//BOT is at 'Power For You', battery level:29 turns_left: 40
	//PATH: battery low,location has battery to sell
	//electricity->buy

	//BOT is at 'Power For You', battery level: 29 turns_left: 20,10kg Bananas
	//PATH: battery not low,carrying fruit
	//within at elec carrying fruit->move -2 (to buyer)

	//battery_capacity=6
	//BOT is at 'Power For You', battery level: 4 turns_left: 40
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//within not carrying fruit->within decide moves->buy 

	//Power For You: will sell 0 kJ of Electricity for $4/kJ
	//BOT is at 'Power For You', battery level: 4 turns_left: 40
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//within not carrying fruit->within decide moves->move -3 (elec)

	//battery_capacity=6
	//BOT is at 'K17 Basement', battery level: 4 turns_left: 40
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//within not carrying fruit->within decide moves->move to elec

	//battery_capacity=6
	//BOT is at 'K17 Basement', battery level: 4 turns_left: 2
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//within not carrying fruit->within decide moves->move 0

	//battery_capacity=17
	//BOT is at 'Power For You', battery level: 10 turns_left: 40
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//within not carrying fruit->within decide moves->move to seller

	//battery_capacity = 62
	//BOT is at 'Power For You', battery level: 32 turns_left: 2
	//PATH:battery not low,area not sold out
	//within not carrying fruit->within decide moves->move 0

	//BOT is at 'Maroubra Melons', battery level: 10 turns_left: 40
	//PATH: battery not low,area not sold out,enough turns, not enough bat
	//electricity->move to elec

	//TESTING IF MAX_MOVES < moves bot wants to use
	//BOT is at 'Matthews B', battery level: 32 turns_left: 30,10kg Durians
	//PATH:battery not low,
	//within carrying fruit->within last resort->move 1(buyer)


	//EXAMPLE 4: MEDIUM WORLD: nothing left to buy in surrounding area
	//John Lions Garden: other
	//Anzac Parade: other
	//Kingsford Oranges: will sell 0 kg of Oranges for $36/kg
	//Maroubra Melons: will sell 0 kg of Watermelons for $7/kg
	//Eastgardens Fruit: will sell 0 kg of Kiwifruit for $10/kg
	//Village Green: other
	//Kensington Kiwifruit: will sell 0 kg of Kiwifruit for $12/kg
	//UNSW Green Waste: will buy 1344 kg of Anything for $1/kg
	//Mathews B: will buy 0 kg of Watermelons for $11/kg
	//Viola Lab: will buy 0 kg of Durian for $160/kg
	//Discount Electrons: will sell 0 kJ of Electricity for $5/kJ
	//K17 Basement: will buy 0 kg of Bananas for $56/kg
	//Science Theatre: will buy 0 kg of Kiwifruit for $23/kg
	//Power For You: will sell 0 kJ of Electricity for $4/kJ
	//Smelly Fruits R Us: will sell 0 kg of Durian for $110/kg
	//Campus Citrus Centre: will sell 0 kg of Oranges for $46/kg
	//Physics Lawn: other
	//Rosebery Orange Grove: will sell 0 kg of Oranges for $56/kg
	//Kora Lab: will buy 0 kg of Durian for $182/kg
	//CLB 8: will buy 0 kg of Kiwifruit for $20/kg
	//Sitar Lab: will buy 0 kg of Durian for $147/kg
	//Botany Produce: will sell 0 kg of Kiwifruit for $11/kg
	//Wentworth Watermelons: will sell 0 kg of Watermelons for $4/kg
	//Clancy Auditorium: will buy 0 kg of Oranges for $84/kg
	//J17 G03: will buy 0 kg of Oranges for $91/kg
	//Mathews C: will buy 0 kg of Kiwifruit for $24/kg
	//Clovelly Watermelons: will sell 0 kg of Watermelons for $8/kg
	//Physics Theatre: will buy 0 kg of Bananas for $34/kg
	//Paddington Banana Plantation: will sell 0 kg of Bananas for $17/kg
	//Bondi Banana Growers: will sell 0 kg of Bananas for $18/kg
	//Quadrangle Lawn: other

	//BOT is at "Rosebery Orange Grove' battery level:32 turns_left:15
	//within not carrying fruit,sold out,move (max)
	
	//Finally, the only difference between these worlds and a large world,
	//is instead of scanning the whole world, the bot only scans plus-minus 
	//2*max_moves from current location.
	//This prevents the bot from wasting moves when the locations are very far
	//apart.  

	//EXAMPLES 5: LARGE WORLD: testing large world strategy path
	//battery_capacity=171
	//maximum_fruit_kg=20
	//maximum_move=10

	//John Lions Garden: other
	//Physics Theatre: will buy 9 kg of Mangos for $41/kg
	//Kensington Cherries: will sell 22 kg of Cherries for $14/kg
	//Anzac Parade: other
	//Paddington Mango Plantation: will sell 19 kg of Mangos for $29/kg
	//CLB 2: will buy 19 kg of Cherries for $26/kg
	//CLB 6: will buy 21 kg of Mangos for $85/kg
	//Quadrangle Lawn: other
	//Mathews B: will buy 0 kg of Lychees for $148/kg
	//Kora Lab: will buy 12 kg of Durian for $88/kg
	//Physics Lawn: other
	//Kingsford Estate: will sell 7 kg of Grapes for $27/kg
	//Keith Burrows Theatre: will buy 11 kg of Cherries for $18/kg
	//Bugle Lab: will buy 15 kg of Limes for $12/kg
	//CLB 4: will buy 18 kg of Lemons for $56/kg
	//Horn Lab: will buy 16 kg of Limes for $10/kg
	///Kensington Solar: will sell 0 kJ of Electricity for $5/kJ
	//Joy's Juices: will buy 19119 kg of Anything for $1/kg
	//Great Grapes: will sell 22 kg of Grapes for $32/kg
	//Discount Electricity: will sell 0 kJ of Electricity for $2/kJ
	//Lovely Limes: will sell 25 kg of Limes for $5/kg
	//J17 G03: will buy 10 kg of Lemons for $90/kg
	//CLB 3: will buy 22 kg of Lemons for $88/kg
	//Oboe Lab: will buy 1 kg of Durian for $141/kg
	//We Love The Smell: will sell 9 kg of Durian for $57/kg
	//Luscious Lychees: will sell 24 kg of Lychees for $73/kg
	//Electricity UNSW: will sell 0 kJ of Electricity for $3/kJ
	//Viola Lab: will buy 20 kg of Durian for $84/kg
	//Drum Lab: will buy 13 kg of Limes for $8/kg
	//CLB 1: will buy 21 kg of Cherries for $32/kg
	//Bondi Mango Growers: will sell 15 kg of Mangos for $31/kg
	//Oud Lab: will buy 9 kg of Limes for $8/kg
	//Campus Cherry Orchard: will sell 8 kg of Cherries for $12/kg
	//Sour Fruit Specialists: will sell 18 kg of Lemons for $38/kg
	//Wentworth Lychees: will sell 0 kg of Lychees for $52/kg
	//Marvellous Mangos: will sell 18 kg of Mangos for $40/kg
	//UNSW Recyling: will buy 14651 kg of Anything for $1/kg
	//Durian Sisters: will sell 12 kg of Durian for $65/kg
	//Smelly Fruits R Us: will sell 12 kg of Durian for $55/kg
	//Clovelly Lychees: will sell 4 kg of Lychees for $62/kg
	//Campus Citrus Centre: will sell 15 kg of Lemons for $47/kg
	//Mathews A: will buy 8 kg of Lychees for $124/kg
	//Campus Green Waste: will buy 14286 kg of Anything for $1/kg
	//Durian For Dummies: will sell 26 kg of Durian for $70/kg
	//K17 Basement: will buy 19 kg of Mangos for $53/kg
	//Bongo Lab: will buy 17 kg of Limes for $8/kg
	//Tabla Lab: will buy 12 kg of Limes for $12/kg
	//CLB 8: will buy 19 kg of Cherries for $23/kg
	//Matthew's Mangos: will sell 22 kg of Mangos for $37/kg
	//Lychees Brothers: will sell 0 kg of Lychees for $52/kg
	//Green Square Vines: will sell 18 kg of Grapes for $36/kg
	//Village Green: other
	//Science Theatre: will buy 11 kg of Cherries for $26/kg
	//Sitar Lab: will buy 18 kg of Durian for $136/kg
	//Eastgardens Fruit: will sell 15 kg of Cherries for $12/kg
	//Rosebery Lemon Grove: will sell 22 kg of Oranges for $35/kg
	//Citrus Specialists: will sell 21 kg of Limes for $5/kg
	//Lyre Lab: will buy 16 kg of Limes for $9/kg
	//Mathews D: will buy 5 kg of Lychees for $140/kg
	//Botany Produce: will sell 19 kg of Cherries for $12/kg
	//CLB 7: will buy 8 kg of Mangos for $75/kg
	//Clancy Auditorium: will buy 16 kg of Lemons for $82/kg
	//Kingsford Lemons: will sell 15 kg of Oranges for $33/kg
 	//Randwick Vineyards: will sell 19 kg of Grapes for $33/kg
  	//Rosebery Vinnyard: will sell 28 kg of Grapes for $39/kg
	//Mathews C: will buy 17 kg of Cherries for $24/kg

	//BOT is at "Clovelly Lychees",battery level:85 turns_left:40
	//PATH: (seller is Smelly Fruits R Us)
	//Within not carrying fruit->within decide moves->move (-1)seller
	
	//BOT is at "Smelly Fruits R Us",battery level:85 turns_left:40,10kg Durian
	//PATH: (seller is Smelly Fruits R Us buyer is Oboe Lab)
	//Within carrying fruit->within at can buy location->move (-15)buyer

	//BOT is at "Drum Lab",battery level:85 turns_left:40,10kg Durian
	//PATH: (seller is Smelly Fruits R Us buyer is Oboe Lab)
	//Within carrying fruit->within at can buy location->move (-5)buyer

	//BOT is at "Oboe Lab",battery level:85 turns_left:40,10kg Durian
	//PATH: (seller is Smelly Fruits R Us buyer is Oboe Lab)
	//Within carrying fruit->within at can sell location->Sell 

	//I also tested my code in multi-bot worlds (large and medium) 
	//The difference between multi-bot and single-bot worlds is that the code
	//now considers n_bots before moving 



}

//ADD YOUR FUNCTIONS HERE 

//considers moves for when bot is at electricity location and carrying fruit
int at_elec_carrying_fruit(struct bot *b){
	//printf("wihin at elec carrying fruit\n");
	int n_loc = 0;
	n_loc = n_locations(b);
	int amount = 0;
	struct location *move_to_location = NULL;
	int move = NOT_COMPLETE;
		if (n_loc <= 4*b->maximum_move){ //smaller world - scan whole world
			move_to_location = find_dearest_buyer_given_fruit(b);
		} else { //scan plus minus two max moves away from current loc
			printf("c\n");
			move_to_location = find_dearest_buyer_given_fruit_large(b,2);
			amount = 3;
			while (move_to_location == NULL){
				move_to_location = find_dearest_buyer_given_fruit_large(b,
																		amount);
				amount++;
			}
		}
		assert(move_to_location != NULL);
		amount = shortest_distance(b->location,move_to_location);
		printf("Move %d\n",amount);
		move = COMPLETE;
	return move;
}

//considers cases when bot is at electricity
int electricity(struct bot *b){
	//printf("Within Electricity\n");
	int move = NOT_COMPLETE;
	int amount = 0;
	struct location *move_to_location = NULL;
	if (strcmp(b->location->fruit,"Electricity") == TRUE && 
		battery_low(b) == TRUE && b->location->quantity > 0){
		amount = b->battery_capacity;
		assert(amount != 0);
		printf("Buy %d\n",amount); //refill battery
		move = COMPLETE;

	} else if (strcmp(b->location->fruit,"Electricity") != TRUE &&
			  battery_low(b) == TRUE){ //move to refill battery
		move_to_location = cheapest_electricity(b);
		if (move_to_location != NULL){
			amount = shortest_distance(b->location,move_to_location);
			assert(amount != 0);
			printf("Move %d\n",amount);
			move = COMPLETE;
		}

	} else if (strcmp(b->location->fruit,"Electricity") == TRUE && 
		battery_low(b) == TRUE && b->location->quantity == 0){
		//current battery location ran out so must move to next elec location
			move_to_location = cheapest_electricity(b);
		if (move_to_location != NULL){
			amount = shortest_distance(b->location,move_to_location);
			assert(amount != 0);
			printf("Move %d\n",amount);
			move = COMPLETE;
		}
	}
	return move;
}

//bot is neither at sell nor buy location (traversing between)
int last_resort(struct bot *b){
	//printf("Within last resort\n");
	int move = NOT_COMPLETE;
	struct location *move_to_location = NULL;
	int amount = 0;

	//printf(" %s %s\n",b->location->name,b->fruit);
	if (n_locations(b) <= 4*b->maximum_move){ //small world - scan whole world
		move_to_location = find_dearest_buyer_given_fruit(b);
	} else {//scan plus minus two max_moves away from current loc
		move_to_location = find_dearest_buyer_given_fruit_large(b,2);
		amount = 3;
		while (move_to_location == NULL){ //if no buyers within limits, 
			//increase limits 
			move_to_location = find_dearest_buyer_given_fruit_large(b,amount);
			amount++;
		}
	}
	assert(move_to_location != NULL);
	amount = shortest_distance(b->location,move_to_location);
	assert(amount != 0);
	printf("Move %d\n",amount);
	move = COMPLETE;
	return move;
}

//considers moves when bot is seller location
int at_can_buy_location(struct bot *b,int energy_required){
	//printf("Within at can buy location\n");
	int move = NOT_COMPLETE;
	struct location *move_to_location = NULL;
	int amount = 0;

	if (b->battery_level < energy_required && b->turns_left >= 6){
		amount = nearest_electricity(b);  //move to refill  battery
		printf("Move %d\n",amount);
		move = COMPLETE;
	}
	if (move == NOT_COMPLETE){ //otherwise
		if (n_locations(b) <= 4*b->maximum_move){//small world - scan whole
			move_to_location = find_dearest_buyer_given_fruit(b);
		} else {//scan plus minus two max_moves away from current loc
			move_to_location = find_dearest_buyer_given_fruit_large(b,2);
			amount = 3;
			while (move_to_location == NULL){//increase limits if no buyer found
				move_to_location = find_dearest_buyer_given_fruit_large(b,amount);
				amount++;
			}
		//printf("move to location is %s\n",move_to_location->name);
		}	
	assert(move_to_location != NULL);
	amount = shortest_distance(b->location,move_to_location);
	assert(amount != 0);
	printf("Move %d\n",amount);
	move = COMPLETE;
	}
	return move;
}

//considers moves when bot is not carrying fruit
void not_carrying_fruit(struct bot *b){
	//printf("Within not carrying fruit\n");
	struct location *seller = NULL;
	struct location *buyer = NULL;
	assert(b != NULL);
	assert(b->location != NULL);
	int move = NOT_COMPLETE;
	int amount = 0;
	int n_loc = n_locations(b);
	int energy_required = 0;
	struct location *move_to_location = NULL;
	struct location *future_location = NULL;

	find_max_profit(b,&seller,&buyer);
	assert(seller != NULL);
	assert(buyer != NULL);
	move_to_location = seller;
	//printf("seller is %s %s\n",seller->name, move_to_location->name);
	future_location = buyer;
	//printf("buyer is %s\n",future_location->name);
	energy_required = fabs(shortest_distance(b->location,move_to_location)*1.0)
				+ fabs(shortest_distance(move_to_location,future_location)*1.0);

	if (seller == buyer){ //surrounding locations are sold out
		//printf("sold out\n");
		printf("Move %d\n",b->maximum_move); //move to new area
		move = COMPLETE;
	}
	decide_moves(b,move_to_location,move,energy_required,future_location);
}

void decide_moves(struct bot *b, struct location *move_to_location,int move ,
				  int energy_required, struct location *future_location){
	//printf("Within decide moves\n");
	int amount = 0;
	if (b->location == move_to_location && move == NOT_COMPLETE &&
	    b->battery_level >= energy_required && 
		b->turns_left >= (energy_required/b->maximum_move) && 
		b->turns_left >= 3){ //already at seller location
		amount = future_location->quantity;
		assert(amount != 0);
		printf("Buy %d\n",amount);
		move = COMPLETE;
	} else if (b->location != move_to_location || move == NOT_COMPLETE){
		if (b->battery_level < energy_required && 
			b->turns_left >= (energy_required/b->maximum_move) && 
			b->turns_left >= 6){
			amount = nearest_electricity(b);
			if(amount == 0){
				printf("Buy %d\n",b->battery_capacity);
			}  //move to refill electricity
			else {
				printf("Move %d\n",amount);
			}
			move = COMPLETE;
		} else if (b->turns_left < (energy_required/b->maximum_move || 
				   b->turns_left < 3)){
		 	//near end of simulation don't buy
			printf("Move 0\n");
		} else {	
			amount = shortest_distance(b->location,move_to_location);
			printf("Move %d\n",amount); //otherwise move to seller location
		}
	}
}
//determines if battery is low
int battery_low(struct bot *b){
	assert(b != NULL);
	int result = FALSE;
	if (b->battery_level <= b->turns_left && b->turns_left > 5 
		&& fruit_left(b) == TRUE){
		if ( b->battery_level < 0.5 * b->battery_capacity){
			result = TRUE;
		}
	}
	return result;
}

//determines if there's is still fruit to buy and sell in world
int fruit_left(struct bot *b){
	assert( b != NULL);
	struct location *curr = b->location;
	struct location *next;
	int first_iteration = TRUE;
	int result = FALSE;
	
	while (first_iteration == TRUE || curr != b->location){
		//if there exists locations which buy and sell one fruit
		if (curr->price > 0 && curr->quantity != 0){
			next = curr->east;
			while (next != curr){
				if (strcmp(next->fruit,curr->fruit) == TRUE && 
					next->price < 0 && next->quantity != 0){ //sells
					result = TRUE;
				}
				next = next->east;
			}
		}
		if (curr->price < 0 && curr->quantity != 0){
			next = curr->east;
			while (next != curr){
				if (strcmp(next->fruit,curr->fruit) == TRUE && 
					next->price > 0 && next->quantity != 0){//buys
					result = TRUE;
				}
				next = next->east;
			}
		}
		curr = curr->east;
		first_iteration = FALSE;
	}
	return result;
}

//finds number of locations in world
int n_locations(struct bot *b){
	assert(b != NULL);
	int num = 1;
	struct location *curr = b->location->east;
	while (curr != b->location){
		num++;
		curr = curr->east;
	}
	return num;
}

//determines if location sells fruit
int can_buy(struct bot *b, struct location *l){
	int result = FALSE;
	int n_bots = 0;
	if (l == b->location){ //n_bots automatically counts self if bot hold fruit
		n_bots = n_bots_at_location_with_fruit(l,b);
	}
	if (l != b->location){
		n_bots = n_bots_at_location_with_fruit(l,b) + 1;
	}
	if (n_bots == 0){ //if bot carries no fruit, add one, so later we don't
	//divide by 0
		n_bots = 1;
	}
	assert(n_bots != 0);
	if (strcmp(l->fruit,b->fruit) == TRUE &&
		l->price < 0 && l->quantity/n_bots > 0){
		result = TRUE;
	}
	return result;
}

//determines if location buys fruit
int can_sell(struct bot *b, struct location *l){
	int result = FALSE;
	int n_bots = 0;
	if (l == b->location){//n_bots automatically counts self if bot hold fruit
		n_bots = n_bots_at_location_with_fruit(l,b);
	}
		if (l != b->location){
		 n_bots = n_bots_at_location_with_fruit(l,b) + 1;
	}
	if (n_bots == 0){ //if bot carries no fruit, add one, so later we don't
	//divide by 0
		n_bots = 1;
	}
	assert(n_bots != 0);
	if (strcmp(l->fruit,b->fruit) == TRUE &&
		l->quantity/n_bots > 0 &&
		l->price > 0){
		result = TRUE;
	}
	return result;
}

//finds shortest distance between two locations
//code based off own lab 11 exercise 3
int shortest_distance(struct location *l1, struct location *l2){
	int distance = 0;
	struct location *curr = l1;
	struct location *east;
	struct location *west;
	int n_east = 0;
	int n_west = 0;
	//loop to east until reach 2nd location
	while (curr != l2){
		n_east++;
		curr = curr->east;
	}
		curr = l1;
	//loop to west count until reach 2nd location 
	while (curr != l2){
		n_west++;
		curr = curr->west;
	}

	//if nearest location with all restrictions is east return pos int
	if (n_east <= n_west){
		distance = n_east;
	}
	// if nearest location is west return neg int 
	if(n_west < n_east){
		distance = -n_west;
	}
	if (l1 != l2){
		assert(distance != 0);
	}
    return distance;
}

//determines if location sells a fruit
int location_sells_fruit(struct location *l){
	int result = FALSE;
	if (strcmp(l->fruit,"Anything") != TRUE && 
		strcmp(l->fruit,"Nothing") != TRUE &&
		strcmp(l->fruit,"Electricity") != TRUE && l->price < 0 &&
		l->quantity > 0){
		result = TRUE;

	}
	return result;
}

//determines if there exists a location which buys specific fruit
int exists_location_buys_same_fruit(struct location *l){
	int result = FALSE;
	struct location *curr = l;
	int first_iteration = TRUE;
	while (first_iteration == TRUE || curr != l){
		if (strcmp(curr->fruit,l->fruit) == TRUE && curr->price > 0 &&
			curr->quantity > 0 && -(l->price) < curr->price){
				result = TRUE; 
		}
		curr = curr->east;
		first_iteration = FALSE;
	}
	return result;
}

//finds location which buys specific fruit
struct location *location_buys_same_fruit(struct location *l){
	struct location *result = NULL;
	struct location *first = NULL;
	struct location *last = NULL;		
	struct location *curr = l;
	int first_iteration = TRUE;
	int first_location = TRUE;

	while (first_iteration == TRUE || curr != l){
		if (strcmp(curr->fruit,l->fruit) == TRUE && curr->price > 0 &&
			curr->quantity > 0 && -(l->price) < curr->price){
			if (first_location == TRUE){
				first = curr;
			} else {
				last = curr;
			}
		}
		curr = curr->east;
		first_iteration = FALSE;
	}
	if (last != NULL){
		if (fabs(shortest_distance(l,first)*1.0) >= 
			fabs(shortest_distance(l,last)*1.0)){
			result = last;
		} else if (fabs(shortest_distance(l,first)*1.0) < 
			fabs(shortest_distance(l,last)*1.0)){
			result = first;
		}
	} else {
		result = first;
	}
	assert(result != NULL);
	return result;
}

//finds distance to nearest electricity location
//code based off own lab 11 exercise 3
int nearest_electricity(struct bot *b){
    int result = 0;
    //if bot is at location that sells electricity and it has some kj to sell print 0
    if (location_has_elec(b->location) == TRUE){
	result = 0;
    }
    //find nearest location 
    //loop to east count until location fits requirements 
    int n_east = 0;
    int n_west = 0;
    struct location *curr = b->location;
	int first_iteration = TRUE;
    while (location_has_elec(curr) == FALSE && (curr != b->location || first_iteration == TRUE)){
	n_east++;
        curr = curr->east;
		first_iteration = FALSE;
    }
    curr = b->location;
    //loop to west count until location fits requirement 
	first_iteration = TRUE;
    while (location_has_elec(curr) == FALSE && (curr != b->location || first_iteration == TRUE)){
	n_west++;
        curr = curr->west;
		first_iteration = FALSE;
    }

    //if nearest location with all restrictions is east return pos int
    if (n_east < n_west){
	result = n_east;
    }
   // if nearest location is west return neg int 
    if(n_west < n_east){
	result = -n_west;
    }
    return result;
}

//determines if location sells electricity
int location_has_elec(struct location *l){
    int result = FALSE;
    if (strcmp(l->fruit,"Electricity") == 0 &&
	l->quantity > 0){
	result = TRUE;

    }
    return result; 
}

//finds location which sells cheapest electricity
struct location *cheapest_electricity(struct bot *b){
	struct location *curr = b->location;
	struct location *cheapest_loc = NULL;
	int first_iteration = TRUE;
	while (first_iteration == TRUE || curr != b->location){
		if(strcmp(curr->fruit,"Electricity") == TRUE && curr->quantity > 0){
			if (cheapest_loc == NULL){
				cheapest_loc = curr;
				
			} else {
				if (curr->price < cheapest_loc->price){
					cheapest_loc = curr;
				}
			}
		}
		curr = curr->east;
		first_iteration = FALSE;
	}
	return cheapest_loc;
}

//loops through world and finds most profitable fruit and corresponding seller
//and buyer
void find_max_profit(struct bot *b, struct location **seller, 
					 struct location **buyer){
	char *seller_name[MAX_FRUIT_TYPES] = {NULL};
	struct location *cheapest = b->location;
	struct location *dearest = NULL;
    char *buyer_name[MAX_FRUIT_TYPES] = {NULL};
	int profit[MAX_FRUIT_TYPES] = {0};
	int n_bots_at_seller = 0;
	int n_bots_at_buyer = 0;
	int i = 0;
	int n_moves = 0;

	struct location *curr = b->location;
	int first_iteration = TRUE;
	if (n_locations(b) <= 4 * b->maximum_move){ //small world-scan through world
		i = small_world(b,first_iteration,curr,seller_name,buyer_name,i);
	} 
	curr = b->location;
	while (n_moves < 2* b->maximum_move){ //reset start back by 2*max_move
		curr = curr->west;
		n_moves++;
	}

	//printf("start search at %s\n",curr->name);
	n_moves = 0;
	if (n_locations(b) > 4 *b->maximum_move){ //large world-scan plus minus 
		i = large_world(b, curr, seller_name, buyer_name,i);
	}
	//printf("%d\n",i);
	int j = 0;
	if (i == 0){ //no good profit place within limits to sell and buy
		*seller = b->location;
		*buyer = b->location;
	}
	else { 
		fill_profit(j,i,b,seller_name,buyer_name,profit);
		compare_profits(i,profit,seller_name,buyer_name,seller,buyer,b);
	}
	
}

//finds profit for all fruits saved in buyer and seller array
void fill_profit(int j, int i, struct bot *b, char *seller_name[],
				 char *buyer_name[],int profit[]){
	struct location *cheapest = b->location;
	struct location *dearest = NULL;
	while (j < i){
		struct location *loop = b->location;
		int first_iteration = TRUE;

		while (first_iteration == TRUE || loop != b->location ){
			if (strcmp(seller_name[j],loop->name) == TRUE){
				cheapest = loop; //finding location with cheapest seller name
			}
			loop = loop->east;
			first_iteration = FALSE;
		}
		loop = b->location;
		first_iteration = TRUE;

		while (first_iteration == TRUE || loop != b->location ){
			if (strcmp(buyer_name[j],loop->name) == TRUE){
				dearest = loop; //finding location with dearest buyer name
			}
			loop = loop->east;
			first_iteration = FALSE;
		}
		profit[j] = cheapest->price + dearest->price;
		j++;
	}
}

//finding cheapest seller and dearest buyer in large world
int large_world(struct bot *b, struct location *curr, char *seller_name[],
				char *buyer_name[], int i){
	struct location *cheapest = b->location;
	struct location *dearest = NULL;
	int n_moves= 0;
	while(n_moves <= 4*b->maximum_move){// scan plus minum 2*max_moves away
		//from current loc
		if (sells_unique_fruit(curr,b) == TRUE){
			//printf("sells unique fruit\n");
			cheapest = curr;
			cheapest = find_cheapest_seller_large(curr,b);
			if (cheapest != NULL){ 
				dearest = find_dearest_buyer_large_no_fruit(curr,b,cheapest);
				assert(cheapest != NULL);
				if (cheapest != NULL && dearest != NULL){
					seller_name[i] = cheapest->name;
					buyer_name[i] = dearest->name;
					//printf("find profit seller %s buyer %s\n",cheapest->name,
					//	   										dearest->name);
					i++;
				}
			}
		}
		curr = curr->east;
		n_moves++;
	}
	return i;
}

//finding cheapest seller and dearest buyer in small world
int small_world(struct bot *b,int first_iteration, struct location *curr, 
				char *seller_name[], char *buyer_name[], int i){

	struct location *cheapest = b->location;
	struct location *dearest = NULL;
		while(first_iteration == TRUE || curr != b->location){
		//scanning through whole world
			if (sells_unique_fruit(curr,b) == TRUE){

				cheapest = curr;
					cheapest = find_cheapest_seller(curr,b);
					dearest = find_dearest_buyer_no_fruit(curr,b,cheapest);
				
			
				assert(cheapest != NULL);
				if (cheapest != NULL && dearest != NULL){

					seller_name[i] = cheapest->name;
					buyer_name[i] = dearest->name;
					i++;
				}
			}
			curr = curr->east;
			first_iteration = FALSE;
		}
	return i;
}

//determines if location sells a fruit, haven't seen before
int sells_unique_fruit(struct location *curr,struct bot *b){
	int result = TRUE;
	//if curr is at location and sells fruit return true
	if (curr == b->location && location_sells_fruit(curr) == TRUE ){
		result = TRUE;
	} 
	if (curr == b->location && location_sells_fruit(curr) != TRUE){
		result = FALSE;
	}
	struct location *compare = b->location;
	if (curr != b->location){
		while (compare != curr){
			if (strcmp(curr->fruit,compare->fruit) == TRUE && compare->price < 0
				&& location_sells_fruit(curr) == TRUE ){
				result = FALSE;
			}
			if (strcmp(curr->fruit,compare->fruit) != TRUE && curr->price > 0 && 					
				location_sells_fruit(curr) == TRUE){
				result = FALSE;
			}
			compare = compare->east;
		}
	}
	if (location_sells_fruit(curr) != TRUE){ //gets rid of buyer locations
		result = FALSE;
	}
	return result;
}

//finds the cheapest seller after scanning whole world
struct location *find_cheapest_seller(struct location *curr, struct bot *b){
	struct location *compare = curr;
	struct location *cheapest = NULL;
	int cheapest_price = MAX_PRICE;
	int n_bots = 0;
	int first_iteration = TRUE;
	while ( first_iteration == TRUE || compare != curr){
		if (compare == b->location){ //automatically includes self
			n_bots = n_bots_at_location(compare,b);
		}
		if (compare != b->location){//add one to include self
			n_bots = n_bots_at_location(compare,b) + 1;
		}
		assert(n_bots != 0);
		if(compare->price > cheapest_price && (compare->quantity/n_bots) > 0 &&
		  location_sells_fruit(compare) == TRUE && compare->price < 0 &&
		  strcmp(compare->fruit,curr->fruit) == TRUE ){
			cheapest_price = compare->price;
			cheapest = compare;
		}
		compare = compare->east;
		first_iteration = FALSE;
	}
	return cheapest;
}

//finds cheapest seller in large world
struct location *find_cheapest_seller_large(struct location *curr,struct bot *b){
	struct location *compare = curr;
	struct location *cheapest = NULL;
	int cheapest_price = MAX_PRICE;
	int n_bots = 0;
	int first_iteration = TRUE;
	int n_moves = 0;
	cheapest = scan_world_east(b,compare,curr,first_iteration,n_moves,
							   n_bots,cheapest_price);
	if(cheapest != NULL){
		cheapest_price = cheapest->price;
	}

	compare = curr;
	n_moves = 0;
	first_iteration = TRUE; 
	while ( first_iteration == TRUE || n_moves <= 2*b->maximum_move){
	//loop through world in west direction to find cheapest seller
		if (compare == b->location){ 
			n_bots = n_bots_at_location(compare,b);
		}
		if (compare != b->location){
			n_bots = n_bots_at_location(compare,b) + 1;
		}
		assert(n_bots != 0);
		if(compare->price > cheapest_price && (compare->quantity/n_bots) > 0 &&
		  location_sells_fruit(compare) == TRUE && compare->price < 0 &&
		  strcmp(compare->fruit,curr->fruit) == TRUE ){
			cheapest_price = compare->price;
			cheapest = compare;
		}
		compare = compare->west;
		first_iteration = FALSE;
		n_moves++;
	}
	return cheapest;
}

//loop through world in east direction to find cheapest seller
struct location *scan_world_east(struct bot *b, struct location *compare,
								struct location *curr,int first_iteration,
								int n_moves,int n_bots,int cheapest_price){
	
	struct location *cheapest = NULL;
	while ( first_iteration == TRUE || n_moves <= 2*b->maximum_move){
		if (compare == b->location){
			n_bots = n_bots_at_location(compare,b);
		}
		if (compare != b->location){
			n_bots = n_bots_at_location(compare,b) + 1;
		}
		//printf("n_bots = %d\n",n_bots);
		assert(n_bots != 0);
		if(compare->price > cheapest_price && (compare->quantity/n_bots) > 0 &&
		   location_sells_fruit(compare)==TRUE && compare->price < 0 &&
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			cheapest_price = compare->price;
			cheapest = compare;
		}
		compare = compare->east;
		first_iteration = FALSE;
		n_moves++;
	}
	return cheapest;
}

//find dearest buyer in small world after scanning whole world
struct location *find_dearest_buyer_no_fruit(struct location *curr, 
											 struct bot *b, 
											 struct location *seller){
	struct location *compare = curr->east;
	struct location *dearest = NULL;
	int dearest_price = 0;
	int n_bots = 0;
	while (compare != curr){
		n_bots = n_bots_at_location(seller,b) + 
				 n_bots_at_location_with_fruit(compare,b) + 1;
		assert(n_bots != 0);
		if(compare->price > dearest_price && (compare->quantity/n_bots) > 0 &&
		   location_buys_fruit(compare) == TRUE && 
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			dearest_price = compare->price;
			dearest = compare;
		}
		compare = compare->east;
	}
	return dearest;
}

//finds dearest buyer in large world when not carrying fruit
struct location *find_dearest_buyer_large_no_fruit(struct location *curr, 
												   struct bot *b, 
										  		   struct location *seller){
	struct location *compare = curr->east;
	struct location *dearest = NULL;
	int dearest_price = 0;
	int n_bots = 0;
	int n_moves = 0;
	//printf("curr is %s seller is %s\n",curr->name,seller->name);
	while (n_moves <= 2*b->maximum_move){ //loop to the east
		n_bots = n_bots_at_location(seller,b) + 
				 n_bots_at_location_with_fruit(compare,b) + 1;
		assert(n_bots != 0);
		if(compare->price > dearest_price && (compare->quantity/n_bots) > 0 &&
		   location_buys_fruit(compare) == TRUE && 
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			dearest_price = compare->price;
			dearest = compare;
		}
		compare = compare->east;
		n_moves++;
	}

	compare = curr->west;
	n_moves = 0;
	while (n_moves <= 1*b->maximum_move){ //loop to the east
		n_bots = n_bots_at_location(seller,b) +
			     n_bots_at_location_with_fruit(compare,b) + 1;
		assert(n_bots != 0);
		if(compare->price > dearest_price && (compare->quantity/n_bots) > 0 &&
		   location_buys_fruit(compare) == TRUE && 
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			dearest_price = compare->price;
			dearest = compare;
		}
		compare = compare->west;
		n_moves++;
	}
	return dearest;
}

//finds dearest buyer in large world when already carrying fruit 
struct location *find_dearest_buyer_large(struct location *curr, 
										  struct bot *b, 
										  struct location *seller){
	struct location *compare = curr->east;
	struct location *dearest = NULL;
	int dearest_price = 0;
	int n_bots = 0;
	int n_moves = 0;
	//printf("curr is %s seller is %s\n",curr->name,seller->name);
	while (n_moves <= 2*b->maximum_move){ //loop to the east
		n_bots = n_bots_at_location_with_fruit(seller,b) + 
				 n_bots_at_location_with_fruit(compare,b) + 1;
		assert(n_bots != 0);
		if(compare->price > dearest_price && (compare->quantity/n_bots) > 0 &&
		   location_buys_fruit(compare) == TRUE && 
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			dearest_price = compare->price;
			dearest = compare;
		}
		compare = compare->east;
		n_moves++;
	}

	compare = curr->west;
	n_moves = 0;
	while (n_moves <= 1*b->maximum_move){ //loop to the west
		n_bots = n_bots_at_location_with_fruit(seller,b) +
			     n_bots_at_location_with_fruit(compare,b) + 1;
		assert(n_bots != 0);
		if(compare->price > dearest_price && (compare->quantity/n_bots) > 0 &&
		   location_buys_fruit(compare) == TRUE && 
		   strcmp(compare->fruit,curr->fruit) == TRUE){
			dearest_price = compare->price;
			dearest = compare;
		}
		compare = compare->west;
		n_moves++;
	}
	return dearest;
}
//determines if location buys fruit
int location_buys_fruit(struct location *l){
	int result = FALSE;
	if (strcmp(l->fruit,"Anything") != TRUE && 
		strcmp(l->fruit,"Nothing") != TRUE &&
		strcmp(l->fruit,"Electricity") != TRUE && l->price > 0 &&
		l->quantity > 0){
		result = TRUE;
	}
	return result;
}

//finds the number of bots at a location;
int n_bots_at_location(struct location *l, struct bot *b){
	int count = 0;
	struct bot_list *curr = l->bots;
	if (l->bots == NULL){
		return 0;
	} else {
		while (curr != NULL){
			count++;
			curr = curr->next;
		}
	}
	return count;
}

//finds  number of bots at a location with a specific fruit
int n_bots_at_location_with_fruit(struct location *l, struct bot *b){
	int count = 0;
	struct bot_list *curr = l->bots;
	if (l->bots == NULL){
		return 0;
	} else {
		while (curr != NULL){

			assert(l != NULL);
			if (curr->bot->fruit != NULL){
				if (strcmp(curr->bot->fruit,l->fruit) == TRUE){
					count++;
				}
			}
			curr = curr->next;
		}
	}
	return count;
}

//finds fruit/locations with max profit
void compare_profits(int i, int profit[], char *seller_name[], 
					 char *buyer_name[], struct location **seller, 
					 struct location **buyer, struct bot *b){
	int j = 1;
	struct location *loop = b->location;
	int first_iteration = TRUE;
	int highest_profit = 0;

	//initialises first fruit 
	highest_profit = first_fruit(b,first_iteration,loop,seller_name,buyer_name,
								 seller,buyer,profit);

	while (j < i){
		if (profit[j] > highest_profit){
			//printf("profit %d\n",profit[j]);
				loop = b->location;
				first_iteration = TRUE;
				while (first_iteration == TRUE || loop != b->location ){
					if (strcmp(seller_name[j],loop->name) == TRUE){
						*seller = loop; //find sell location from name
					//printf("next seller %s\n",loop->name);
					}
					loop = loop->east;
					first_iteration = FALSE;
				}
				loop = b->location;
				first_iteration = TRUE;
				while (first_iteration == TRUE || loop != b->location ){
					if (strcmp(buyer_name[j],loop->name) == TRUE){
						*buyer = loop; //find buy location from name
					//printf("next buyer %s\n",loop->name);
					}
					loop = loop->east;
					first_iteration = FALSE;
				}
				highest_profit = profit[j];
		}
		j++;
	}
}

//initialises first fruit/profit
int first_fruit(struct bot *b,int first_iteration,struct location *loop,
				char *seller_name[],char *buyer_name[],
				struct location **seller,struct location **buyer, int profit[]){
	int highest_profit;
	while (first_iteration == TRUE || loop != b->location ){
		if (strcmp(seller_name[0],loop->name) == TRUE){
			*seller = loop; //finds sell location from name
			//printf("profit %d\n",profit[0]);
			//printf("first seller %s\n",loop->name);
		}
		loop = loop->east;
		first_iteration = FALSE;
	}
	
	loop = b->location;
	first_iteration = TRUE;
	while (first_iteration == TRUE || loop != b->location ){
		if (strcmp(buyer_name[0],loop->name) == TRUE){
			//printf("first buyer %s\n",loop->name);
			*buyer = loop; //finds buy location from name

		}
		loop = loop->east;
		first_iteration = FALSE;
	}
	highest_profit = profit[0];
	return highest_profit;
}

//finds dearest buy with a specific fruit in small world after scanning whole
//world
struct location *find_dearest_buyer_given_fruit(struct bot *b){
	struct location *dearest = NULL;
	int dearest_price = 0;
	int n_bots = 0;
	int first_iteration = TRUE;
 	struct location *curr = b->location;
	while (first_iteration == TRUE || curr != b->location){
		if (curr == b->location){
			n_bots = n_bots_at_location_with_fruit(curr,b);
		} else if (curr != b->location){
			n_bots = n_bots_at_location_with_fruit(curr,b) + 
					 n_bots_at_location_with_fruit(b->location,b);
		}
		if (n_bots == 0){
			n_bots = 1;
		}
		assert(n_bots != 0);
		if (curr->price > dearest_price && location_buys_fruit(curr) == TRUE &&
			strcmp(curr->fruit,b->fruit) == TRUE && (curr->quantity/n_bots) > 0){
			dearest_price = curr->price;
			dearest = curr;
		}
		curr = curr->east;
		first_iteration = FALSE;

	}
	return dearest;
}

//finds dearest buyer with specific fruit in large world
struct location *find_dearest_buyer_given_fruit_large(struct bot *b,int amount){
	struct location *dearest = NULL;
	int dearest_price = 0;
	int n_bots = 0;
	int n_moves = 0;
 	struct location *curr = b->location;
		//printf("amount is %d\n",amount);
		while (n_moves <= amount*b->maximum_move){ //loop to the east
		//printf("curr is %s\n",curr->name);
			if (curr == b->location){
			n_bots = n_bots_at_location_with_fruit(curr,b);
			} else if (curr != b->location){
				n_bots = n_bots_at_location_with_fruit(curr,b) + 
						 n_bots_at_location_with_fruit(b->location,b);
			}
			if (n_bots == 0){
				n_bots = 1;
			}
		assert(n_bots != 0);
		if (curr->price > dearest_price && location_buys_fruit(curr) == TRUE &&
			strcmp(curr->fruit,b->fruit) == TRUE && (curr->quantity/n_bots) > 0){
			dearest_price = curr->price;
			dearest = curr;
		}
		curr = curr->east;
		n_moves++;
	}
	curr = b->location;
	n_moves = 0;
	while (n_moves <= amount*b->maximum_move){//loop to the west
		if (curr == b->location){
			n_bots = n_bots_at_location_with_fruit(curr,b);
		} else if (curr != b->location){
			n_bots = n_bots_at_location_with_fruit(curr,b) + 
					 n_bots_at_location_with_fruit(b->location,b);
		}
		if (n_bots == 0){
			n_bots = 1;
		}
		assert(n_bots != 0);
		if (curr->price > dearest_price && location_buys_fruit(curr) == TRUE &&
			strcmp(curr->fruit,b->fruit) == TRUE && (curr->quantity/n_bots) > 0){
			dearest_price = curr->price;
			dearest = curr;
		}
		curr = curr->west;
		n_moves++;
	}
	return dearest;
}

