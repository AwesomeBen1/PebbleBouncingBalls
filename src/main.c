// Bouncing Balls
// Made by Ben Chapman-Kish on 2015-04-03 from 18:44 to 19:00
#include <pebble.h>
// This would have been so much easier and quicker if cloudpebble had code completion or error detection.

#define NUM_BALLS 10
#define TIMER_MS 20


// After hours of debugging, I finally discovered that floats don't work
// in Pebble C, so point/speed values are now hundreths of a pixel.
typedef struct {
	uint16_t radius;
	int xpos;
	int ypos;
	int xspeed;
	int yspeed;
} Ballstruct;

Ballstruct balls[NUM_BALLS];

static Window *s_window;
static Layer *s_layer;

static int randrange(int min_val, int max_val) {
	// It's like random.randrange in python, but inclusive, and without default parameters!
	return (rand() % (1 + max_val - min_val)) + min_val;
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorWhite);
	for (int i = 0; i < NUM_BALLS; ++i) {
		Ballstruct *ball = &balls[i];
		GPoint ballpoint = {
			.x = (int16_t)(ball->xpos / 100),
			.y = (int16_t)(ball->ypos / 100)
		};
		graphics_fill_circle(ctx, ballpoint, ball->radius);
	}
}

static void timer_callback(void *context) {
	for (int i = 0; i < NUM_BALLS; ++i) {
		Ballstruct *ball = &balls[i];
		ball->xpos += ball->xspeed;
		ball->ypos += ball->yspeed;
		int txpos = (int16_t)(ball->xpos / 100);
		// X bounce detection
		if ((txpos + ball->radius >= 144 && ball->xspeed > 0) || (txpos - ball->radius <= 0 && ball->xspeed < 0)) {
			ball->xspeed *= -1;
		}
		int typos = (int16_t)(ball->ypos / 100);
		// Y bounce detection
		if ((typos + ball->radius >= 154 && ball->yspeed > 0) || (typos - ball->radius <= 0 && ball->yspeed < 0)) {
			ball->yspeed *= -1;
		}
	}
	layer_mark_dirty(s_layer);
  app_timer_register(TIMER_MS, timer_callback, NULL);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	//GPoint center = grect_center_point(&bounds);
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Initializing balls.");
	for (int i = 0; i < NUM_BALLS; ++i) {
		Ballstruct *ball = &balls[i];
		ball->radius = randrange(5, 20);
		ball->xpos = randrange(ball->radius, bounds.size.w - ball->radius) * 100;
		ball->ypos = randrange(ball->radius, bounds.size.h - ball->radius) * 100;
		ball->xspeed = randrange(20, 400);
		ball->yspeed = randrange(20, 400);
	}
	
	s_layer = layer_create(bounds);
	layer_set_update_proc(s_layer, layer_update_proc);
	layer_add_child(window_layer, s_layer);
}

static void window_unload(Window *window) {
	layer_destroy(s_layer);
}

static void init() {
	s_window = window_create();
	window_set_background_color(s_window, GColorBlack);
	window_set_window_handlers(s_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
	window_stack_push(s_window, true);
	app_timer_register(TIMER_MS, timer_callback, NULL);
}

static void deinit() {
	window_destroy(s_window);
}

int main() {
	init();
	app_event_loop();
	deinit();
}