#pragma once

/* Function prototypes */
void framebf_init(unsigned int physical_width,
                  unsigned int physical_height,
                  unsigned int virtual_width,
                  unsigned int virtual_height,
                  unsigned int virtual_x_offset,
                  unsigned int virtual_y_offset);
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawLineARGB32(int x1, int y1, int x2, int y2, unsigned int attr);
void drawLineWithThicknessARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int thickness);
void drawCircleARGB32(int x_center, int y_center, int radius, unsigned int attr, int fill);
void drawStarARGB32(int centerX, int centerY, int outerRadius, int innerRadius, unsigned int attr);
void drawImage(const unsigned long *img, int x, int y, int img_pixels_width, int img_pixels_height);
void drawVideo(const unsigned long **video, int x, int y, int video_pixels_width, int video_pixels_height, int video_length);
