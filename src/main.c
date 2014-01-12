#include <pebble.h>

#define REPEATING_CLICK_INTERVAL 100
#define MAX_NUM_LENGTH 4
static Window *window;

static TextLayer *textLayer;
static TextLayer *layer2;
static Layer *imglayer;

static GBitmap *image;

static char text_buffer [5];
static char text_layer2[13+MAX_NUM_LENGTH];
static int num_gen=10;

/*
char *intost(int num, char *string)
{
	int i = 0, temp_num = num, length = 0;
	
	if (num==0) {string[0]='0'; string[1]='\0'; return string;}
	
	if(num > 0) {
		while(temp_num) {
			temp_num /= 10;
			length++;
		}
		
		for(i = 0; i < length; i++) {
		 	string[(length-1)-i] = '0' + (num % 10);
			num /= 10;
		}
		string[i] = '\0'; 
	}
	else
	{string[0]='B'; string[1]='a'; string[2]='d'; string[3]='\0';}
	
	return string;
}
*/

void layer_update_callback(Layer *layer, GContext* ctx) {

  GRect destination = image->bounds;

  destination.origin.y = 0;
  destination.origin.x = 121;

  graphics_draw_bitmap_in_rect(ctx, image, destination);


}


// Modify these common button handlers

static int numClicks = 0;
void btn_up_handler(ClickRecognizerRef recognizer, void *context) {
        numClicks = 0;
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	numClicks++;
	if (numClicks<21) num_gen++; else num_gen+=10;
	//intost(num_gen,text_layer2+12);
	snprintf(text_layer2+12,13+MAX_NUM_LENGTH,"%d",num_gen);
	text_layer_set_text(layer2, text_layer2);
}




void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {

	if (num_gen>1) {
		numClicks++;
		if (numClicks<21) num_gen--; else num_gen-=10;
		if (num_gen<=0) num_gen=1;
		//intost(num_gen,text_layer2+12);
		snprintf(text_layer2+12,13+MAX_NUM_LENGTH,"%d",num_gen);
		text_layer_set_text(layer2, text_layer2);
	}
}

static int timer_delay=10*100;

void timer_call() {
	
 	//intost(rand()%(num_gen)+1,text_buffer);
	snprintf(text_buffer,MAX_NUM_LENGTH+1,"%d",rand()%(num_gen)+1);

	text_layer_set_text(textLayer, text_buffer);
    if (timer_delay < 300*100 )
    {
        timer_delay=timer_delay/0.7;
        app_timer_register(timer_delay/100, timer_call, NULL);
    }
    else
    {
        timer_delay=10*100;
    }

}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {

timer_call();
}




void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {

	num_gen=10;
	
	//intost(num_gen,text_layer2+12);
	snprintf(text_layer2+12,13+MAX_NUM_LENGTH,"%d",num_gen);
	
	text_layer_set_text(layer2, text_layer2);
	
}



void click_config_provider(Window *window) {

	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
	window_long_click_subscribe(BUTTON_ID_SELECT, 700, (ClickHandler) select_long_click_handler, NULL);

	window_raw_click_subscribe(BUTTON_ID_UP, NULL, btn_up_handler, NULL);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEATING_CLICK_INTERVAL, (ClickHandler) up_single_click_handler);
    window_raw_click_subscribe(BUTTON_ID_DOWN, NULL, btn_up_handler, NULL);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEATING_CLICK_INTERVAL, (ClickHandler) down_single_click_handler);
	
}


// Standard app initialisation

void handle_init() {

  window = window_create();
  window_stack_push(window, true /* Animated */);

	  // Initialize the layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

	layer2 = text_layer_create(GRect(0, 0, 121, 50));
	text_layer_set_text_alignment(layer2, GTextAlignmentCenter);
	 text_layer_set_font(layer2, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
 layer_add_child(window_layer, text_layer_get_layer(layer2));

	//	text_layer_set_text(layer2, "Gen. limit: 10");
	snprintf(text_layer2+12,13+MAX_NUM_LENGTH,"%d",num_gen);
	text_layer_set_text(layer2, text_layer2);
	
		textLayer = text_layer_create(GRect(0, 55, 121, 30));
	text_layer_set_text_alignment(textLayer, GTextAlignmentCenter);
  text_layer_set_font(textLayer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
layer_add_child(window_layer, text_layer_get_layer(textLayer));
		text_layer_set_text(textLayer, "0");
	
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_1);
	

  imglayer = layer_create(bounds);

  // Set up the update layer callback
  layer_set_update_proc(imglayer, layer_update_callback);

  // Add the layer to the window for display
  layer_add_child(window_layer, imglayer);


  // Attach our desired button functionality
   window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);
}

void handle_deinit() {

  text_layer_destroy(layer2);
  text_layer_destroy(textLayer);
  gbitmap_destroy(image);
  window_destroy(window);
  layer_destroy(imglayer);
}

int main(void) {
	strcpy(text_layer2, "Gen. limit: ");
	handle_init();
	srand(time(0));
	app_event_loop();
	handle_deinit();
}