/**
* @file graphics.h
* @author Cem Gokmen
* @date March 5, 2018
* @brief A graphics library for drawing geometry, for Homework 8 of Georgia Tech
* CS 2110, Spring 2018.
*/

#include "myLib.h"
#include <stdlib.h>
#include <stdio.h>

#define OFFSET(r, c, numcols) ((r) * (numcols) + (c))

#define UNUSED(x) (void)(x)

void setPixel(int x, int y, u16 color);

void drawHollowRect(int x, int y, int width, int height, u16 color);

void drawRectangle(int row, int col, int width, int height, volatile unsigned int color);

void drawImage3(int row, int col, int width, int height, const u16* image);

void gameOver();

int collision(u16 dawg_x, u16 dawg_y, u16 dawg_width, u16 dawg_height, u16 jacket_x, u16 jacket_y, u16 jacket_width, u16 jacket_height);


