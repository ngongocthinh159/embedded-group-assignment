// ----------------------------------- framebf.c -------------------------------------
#include "lib/framebf.h"
#include "lib/mbox.h"
#include "lib/uart.h"
#include "lib/math.h"
#include "lib/timer.h"

// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32

// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

// Default screen info
const unsigned int DEFAULT_PHYSICAL_WIDTH = 1024;
const unsigned int DEFAULT_PHYSICAL_HEIGHT = 768;
const unsigned int DEFAULT_VIRTUAL_WIDTH = 1024;
const unsigned int DEFAULT_VIRTUAL_HEIGHT = 768;
const unsigned int DEFAULT_VIRTUAL_X_OFFSET = 0;
const unsigned int DEFAULT_VIRTUAL_Y_OFFSET = 0;

// Actual screen info
unsigned int width, height, pitch;

/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;

// Const
const double EPS = 1.e-6;

/**
 * Set screen resolution to 1024x768
 */
void framebf_init(unsigned int physical_width,
                  unsigned int physical_height,
                  unsigned int virtual_width,
                  unsigned int virtual_height,
                  unsigned int virtual_x_offset,
                  unsigned int virtual_y_offset)
{
    mBuf[0] = 35 * 4; // Length of message in bytes
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_SETPHYWH;      // Set physical width-height
    mBuf[3] = 8;                      // Value size in bytes
    mBuf[4] = 0;                      // REQUEST CODE = 0
    mBuf[5] = physical_width; // Value(width)
    mBuf[6] = physical_height; // Value(height)

    mBuf[7] = MBOX_TAG_SETVIRTWH;      // Set virtual width-height
    mBuf[8] = 8;                       // Value size in bytes
    mBuf[9] = 0;                       // REQUEST CODE = 0
    mBuf[10] = virtual_width;  // Value(width)
    mBuf[11] = virtual_height; // Value(height)

    mBuf[12] = MBOX_TAG_SETVIRTOFF;      // Set virtual offset
    mBuf[13] = 8;                        // Value size in bytes
    mBuf[14] = 0;                        // REQUEST CODE = 0
    mBuf[15] = virtual_x_offset; // x offset
    mBuf[16] = virtual_y_offset; // y offset

    mBuf[17] = MBOX_TAG_SETDEPTH; // Set color depth
    mBuf[18] = 4;                 // Value size in bytes
    mBuf[19] = 0;                 // REQUEST CODE = 0
    mBuf[20] = COLOR_DEPTH;       // Bits per pixel

    mBuf[21] = MBOX_TAG_SETPXLORDR; // Set pixel order
    mBuf[22] = 4;                   // Value size in bytes
    mBuf[23] = 0;                   // REQUEST CODE = 0
    mBuf[24] = PIXEL_ORDER;         // 0x0: BGR, 0x1: RGB (from MSB to LSB)

    mBuf[25] = MBOX_TAG_GETFB; // Get frame buffer
    mBuf[26] = 8;              // Value size in bytes
    mBuf[27] = 0;              // REQUEST CODE = 0
    mBuf[28] = 16;             // alignment in 16 bytes
    mBuf[29] = 0;              // will return Frame Buffer size in bytes

    mBuf[30] = MBOX_TAG_GETPITCH; // Get pitch
    mBuf[31] = 4;                 // Value size in bytes
    mBuf[32] = 0;                 // REQUEST CODE = 0
    mBuf[33] = 0;                 // Will get pitch value here

    mBuf[34] = MBOX_TAG_LAST;

    // Call Mailbox
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) // mailbox call is successful ?
        && mBuf[20] == COLOR_DEPTH          // got correct color depth ?
        && mBuf[24] == PIXEL_ORDER          // got correct pixel order ?
        && mBuf[28] != 0                    // got a valid address for frame buffer ?
    )
    {
        /* Convert GPU address to ARM address (clear higher address bits)
         * Frame Buffer is located in RAM memory, which VideoCore MMU
         * maps it to bus address space starting at 0xC0000000.
         * Software accessing RAM directly use physical addresses
         * (based at 0x00000000)
         */
        mBuf[28] &= 0x3FFFFFFF;

        // Access frame buffer as 1 byte per each address
        fb = (unsigned char *)((unsigned long)mBuf[28]);
        uart_puts("Got allocated Frame Buffer at RAM physical address: ");
        uart_hex(mBuf[28]);
        uart_puts("\n");

        uart_puts("Frame Buffer Size (bytes): ");
        uart_dec(mBuf[29]);
        uart_puts("\n");

        width = mBuf[5];  // Actual physical width
        height = mBuf[6]; // Actual physical height
        pitch = mBuf[33]; // Number of bytes per line

        uart_puts("Actual width: ");
        uart_dec(width);
        uart_puts("\n");

        uart_puts("Actual height: ");
        uart_dec(height);
        uart_puts("\n");

        uart_puts("Actual pitch: ");
        uart_dec(pitch);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to get a frame buffer with provided setting\n");
    }
}

void drawPixelARGB32(int x, int y, unsigned int attr)
{
    int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);

    /* //Access and assign each byte
     *(fb + offs ) = (attr >> 0 ) & 0xFF; //BLUE (get the least significant byte)
     *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
     *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
     *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
     */

    // Access 32-bit together
    *((unsigned int *)(fb + offs)) = attr;
}

void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
    // Clip the rectangle's coordinates to the screen boundaries
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= width)
        x2 = width - 1;
    if (y2 >= height)
        y2 = height - 1;

    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixelARGB32(x, y, attr);
            else if (fill)
                drawPixelARGB32(x, y, attr);
        }
    }
}

void drawLineARGB32(int x1, int y1, int x2, int y2, unsigned int attr)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        drawPixelARGB32(x1, y1, attr);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void drawLineWithThicknessARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int thickness)
{
    // Calculate the direction of the line
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Normalize the direction vector
    float length = sqrt(dx * dx + dy * dy);
    float offsetX = (float)(-dy) / length;
    float offsetY = (float)(dx) / length;

    // Half the thickness so we can draw lines on both sides
    float halfThickness = thickness / 2.0f;

    // Loop through the thickness and draw multiple parallel lines
    for (float i = -halfThickness; i <= halfThickness; i += 0.5f)
    {
        int xOffset = (int)(offsetX * i);
        int yOffset = (int)(offsetY * i);

        // Draw a line with the current offset
        drawLineARGB32(x1 + xOffset, y1 + yOffset, x2 + xOffset, y2 + yOffset, attr);
    }
}

void drawCircleARGB32(int x_center, int y_center, int radius, unsigned int attr, int fill)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x)
    {
        if (fill)
        {
            // Draw horizontal lines to fill the circle
            for (int i = x_center - x; i <= x_center + x; i++)
            {
                if (i >= 0 && i < width)
                {
                    if (y_center + y >= 0 && y_center + y < height)
                        drawPixelARGB32(i, y_center + y, attr); // Octant 1 and 2
                    if (y_center - y >= 0 && y_center - y < height)
                        drawPixelARGB32(i, y_center - y, attr); // Octant 7 and 8
                }
            }
            for (int i = x_center - y; i <= x_center + y; i++)
            {
                if (i >= 0 && i < width)
                {
                    if (y_center + x >= 0 && y_center + x < height)
                        drawPixelARGB32(i, y_center + x, attr); // Octant 3 and 4
                    if (y_center - x >= 0 && y_center - x < height)
                        drawPixelARGB32(i, y_center - x, attr); // Octant 5 and 6
                }
            }
        }
        else
        {
            // Draw pixels on the circumference only
            if (x_center + x >= 0 && x_center + x < width)
            {
                if (y_center + y >= 0 && y_center + y < height)
                    drawPixelARGB32(x_center + x, y_center + y, attr); // Octant 1
                if (y_center - y >= 0 && y_center - y < height)
                    drawPixelARGB32(x_center + x, y_center - y, attr); // Octant 8
            }
            if (x_center - x >= 0 && x_center - x < width)
            {
                if (y_center + y >= 0 && y_center + y < height)
                    drawPixelARGB32(x_center - x, y_center + y, attr); // Octant 2
                if (y_center - y >= 0 && y_center - y < height)
                    drawPixelARGB32(x_center - x, y_center - y, attr); // Octant 7
            }
            if (x_center + y >= 0 && x_center + y < width)
            {
                if (y_center + x >= 0 && y_center + x < height)
                    drawPixelARGB32(x_center + y, y_center + x, attr); // Octant 3
                if (y_center - x >= 0 && y_center - x < height)
                    drawPixelARGB32(x_center + y, y_center - x, attr); // Octant 6
            }
            if (x_center - y >= 0 && x_center - y < width)
            {
                if (y_center + x >= 0 && y_center + x < height)
                    drawPixelARGB32(x_center - y, y_center + x, attr); // Octant 4
                if (y_center - x >= 0 && y_center - x < height)
                    drawPixelARGB32(x_center - y, y_center - x, attr); // Octant 5
            }
        }

        x++;

        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
    }
}

/* Functions to display text on the screen */
// NOTE: zoom = 0 will not display the character
// void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom)
// {
//     unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

//     for (int i = 1; i <= (FONT_HEIGHT*zoom); i++) {
// 		for (int j = 0; j< (FONT_WIDTH*zoom); j++) {
// 			unsigned char mask = 1 << (j/zoom);
//             if (*glyph & mask) { //only draw pixels belong to the character glyph
// 			    drawPixelARGB32(x + j, y + i, attr);
//             }
// 		}
// 		glyph += (i % zoom) ? 0 : FONT_BPL;
//     }
// }

// void drawString(int x, int y, char *str, unsigned int attr, int zoom)
// {
//     while (*str) {
//         if (*str == '\r') {
//             x = 0;
//         } else if (*str == '\n') {
//             x = 0;
// 			y += (FONT_HEIGHT*zoom);
//         } else {
//             drawChar(*str, x, y, attr, zoom);
//             x += (FONT_WIDTH*zoom);
//         }
//         str++;
//     }
// }

// Function to draw line
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr)
{
	for (int x = x1; x <= x2; x++ ){
        int y = (float)(y1 - y2)/(x1 - x2)*(x - x1) + y1;
        drawPixelARGB32(x, y, attr);
    }
}

// Function to draw circle
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr, int fill)
{
    //Draw the circle when going on x side
    for (int x = center_x - radius; x <= center_x + radius; x++) {
        // Calculate the corresponding y values using the circle equation
        int dy = sqrt(radius * radius - (x - center_x) * (x - center_x)); 
        int upper_y = center_y + dy;
        int lower_y = center_y - dy;

        drawPixelARGB32(x, upper_y, attr);
        drawPixelARGB32(x, lower_y, attr);

        // Fill the circle, draw a line between lower_y and upper_y
        if (fill) {
            for (int y = lower_y; y <= upper_y; y++) {
                drawPixelARGB32(x, y, attr);
            }
        }
    }

    /* Also draw the circle border when going on y side (
    since some points may be missing due to inaccurate calculation above) */

    for (int y = center_y - radius; y <= center_y + radius; y++) {
        // Calculate the corresponding x values using the circle equation
        int dx = sqrt(radius * radius - (y - center_y) * (y - center_y)); 

        int left_x = center_x - dx;
        int right_x = center_x + dx;

        drawPixelARGB32(left_x, y, attr);
        drawPixelARGB32(right_x, y, attr);
    }
}

// example usage
// img: unsigned long img_name[] = {0x1, 0x2, 0x1, 0x2};
// drawImage(img_name, 0, 0, 2, 2);
void drawImage(const unsigned long *img, int x, int y, int img_pixels_width, int img_pixels_height) {
    for (int cur_y = y, i = 0; cur_y < y + img_pixels_height; cur_y++, i++) {
        for (int cur_x = x, j = 0; cur_x < x + img_pixels_width; cur_x++, j++) {
            if (0 <= cur_x && cur_x < width && 0 <= cur_y && cur_y < height) {
                drawPixelARGB32(cur_x, cur_y, img[img_pixels_width*i + j]);
            }
        }
    }
}

// example usage
// img 1: unsigned long img_1[] = {0x1, 0x2, 0x1, 0x2};
// img 2: unsigned long img_2[] = {0x1, 0x2, 0x1, 0x2};
// video: const unsigned long* video_array = {img_1, img_2};
// drawVideo(video_array, 0, 0, 2, 2, 2);
void drawVideo(const unsigned long **video, int x, int y, int video_pixels_width, int video_pixels_height, int video_length) {
    for (int i = 0; i < video_length; i++) {
        set_wait_timer(1, 200);
        drawImage(video[i], x, y, video_pixels_width, video_pixels_height);
        set_wait_timer(0, 200);
    }
}