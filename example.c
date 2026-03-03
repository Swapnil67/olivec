#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "olive.c"

#define WIDTH 800
#define HEIGHT 600

#define COLS 8
#define ROWS 6
#define CELL_HEIGHT (HEIGHT / ROWS)
#define CELL_WIDTH (WIDTH / COLS)


// #define WHITE_COLOR 0xFFFFFFFF
#define BACKGROUND_COLOR 0xFF202020

static uint32_t pixels[HEIGHT * WIDTH];


void swap_int(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void olivec_draw_line(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
  int x1, int y1, int x2, int y2, uint32_t color) {
  // * y = k * x + c

  // * y1 = k * x1 + c
  // * y2 = k * x2 + c

  // * c = y1 - k * x1
  // * y2 = k * x2 + (y1 - k * x1)
  // * y2 = k * x2 + y1 - k * x1

  // * y2 = k * (x2 - x1) + y1
  // * Subtract y1 from both sides
  // * y2 - y1 = k * (x2 - x1) 

  // * c = y1 - k * x1;                -- value of 'c'
  // * k = (y2 - y1) / (x2 - x1)       -- value of 'k'

  int dx = x2 - x1;
  int dy = y2 - y1;
  printf("dy: %d, dx : %d\n", dy, dx);
  
  if (dx != 0) {
    int k = dy / dx;
    // int c = y1 - (k * x1);
    int c = y1 - (dy * x1) / dx;

    printf("k: %d, c : %d\n", k, c);
    if (x1 > x2)
      swap_int(&x1, &x2);
      
    for (int x = x1; x <= x2; ++x) {
      if (x >= 0 && x < (int)pixels_width) {
        // int y = (dy * x) / dx + c;
        int sy1 = (dy * x) / dx + c;
        int sy2 = (dy * (x + 1)) / dx + c;
        if (sy1 > sy2)
          swap_int(&sy1, &sy2);
        for (int y = sy1; y <= sy2; ++y) {
          // printf("y: %d\n", y);
          if (y >= 0 && y < (int)pixels_height) {
            pixels[y * pixels_width + x] = color;
          }
        }
      }
    }
  } else {
    // * Check 'x' bounds
    int x = x1;
    if (x < 0 && x >= (int)pixels_width) {
      return;
    }

    if (y1 > y2)
      swap_int(&y1, &y2);

    for (int y = y1; y < y2; ++y) {
      // * Check 'y' bounds
      if (y >= 0 && y < (int)pixels_height) {
        pixels[y * pixels_width + x] = color;
      }
    }
  }

}

bool check_example(void) {
  // * Fill the pixels with bg color
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      uint32_t color = BACKGROUND_COLOR;
      if ((x + y) % 2 == 0) {
        color = 0xFF2020FF;
      }
      olivec_fill_rect(pixels, WIDTH, HEIGHT, x * CELL_WIDTH, y * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, color);
    }
  }

  // * Save the pixels as ppm image
  const char *filepath = "checker.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, filepath);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", filepath, strerror(errno));
    return false;
  }
  
  return true;
}

bool circle_example(void) {
  // * Fill the pixels with bg color
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      size_t radius = CELL_WIDTH;
      if (CELL_HEIGHT < radius)
        radius = CELL_HEIGHT;
      olive_fill_circle(pixels,
                        WIDTH, HEIGHT,
                        x * CELL_WIDTH + CELL_WIDTH / 2,
                        y * CELL_HEIGHT + CELL_HEIGHT / 2,
                        radius / 2,
                        0xFF0000FF);
    }
  }


  // * Save the pixels as ppm image
  const char *filepath = "circle.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, filepath);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", filepath, strerror(errno));
    return false;
  }

  return true;
}

bool lines_example(void) {
  // * Fill the pixels with bg color
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

  // * Draw line
  olivec_draw_line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, 0xFF2020FF);
  olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH, 0, 0, HEIGHT, 0xFF2020FF);
  olivec_draw_line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH/4, HEIGHT, 0xFF2020FF);
  olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH/4, 0, 0, HEIGHT, 0xFF2020FF);
  olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH / 4 * 3, 0, WIDTH, HEIGHT, 0xFF2020FF);
  olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH, 0, WIDTH / 4 * 3, HEIGHT, 0xFF2020FF);

  // * Save the pixels as ppm image
  const char *filepath = "lines.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, filepath);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", filepath, strerror(errno));
    return false;
  }
  return true;  
}

int main(void) {
  if (!check_example())
    return -1;

  if (!circle_example())
    return -1;

  if (!lines_example())
    return -1;
  return 0;
}