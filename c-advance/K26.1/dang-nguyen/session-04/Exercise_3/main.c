#include "console_display.h"
#include "dummy_display.h"

#define BAUD_RATE   (10U)
#define PIXEL_NUM   (4U)

#define APP_SUCCESS (0U)
#define APP_FAILURE (1U)

typedef struct st_pixel_info
{
    uint16_t x;
    uint16_t y;
    uint8_t  colour;
} st_pixel_info_t;

static int32_t draw_rectangle(st_i_display_t *p_disp);

st_display_config_t *p_display_config;

static int32_t draw_rectangle(st_i_display_t *p_disp)
{
    static st_pixel_info_t pixel_info[PIXEL_NUM] =
    {
        [0] = { .x = 0, .y = 0, .colour = 1 },
        [1] = { .x = 1, .y = 0, .colour = 1 },
        [2] = { .x = 0, .y = 1, .colour = 1 },
        [3] = { .x = 1, .y = 1, .colour = 1 }
    };
    int32_t ret = APP_SUCCESS;

    if (NULL == p_disp)
    {
        printf("[%s]: p_disp is NULL!\n", __func__);
        ret = APP_FAILURE;
    }
    else if (NULL == p_disp->draw_pixel)
    {
        printf("[%s]: p_disp->draw_pixel is NULL!\n", __func__);
        ret = APP_FAILURE;
    }
    else
    {
        for (uint32_t i = 0U; i < PIXEL_NUM; i++)
        {
            p_disp->draw_pixel(pixel_info[i].x, pixel_info[i].y, pixel_info[i].colour);
        }
    }

    return ret;
}

int32_t main(void)
{
    int32_t ret = APP_SUCCESS;

    if (NULL == console_display.init)
    {
        printf("console_display.init() is not set!\n");
        ret = APP_FAILURE;
    }
    else if (NULL == dummy_display.init)
    {
        printf("dummy_display.init() is not set!\n");
        ret = APP_FAILURE;
    }
    else
    {
        p_display_config = console_config_create(BAUD_RATE);

        if (NULL != p_display_config)
        {
            console_display.init(p_display_config);
            ret = draw_rectangle(&console_display);
        
            if (APP_SUCCESS == ret)
            {
                dummy_display.init(p_display_config);
                ret = draw_rectangle(&dummy_display);
                printf("Dummy display was called %u times.\n", get_draw_count());
            }
        
            console_config_destroy(p_display_config);
            p_display_config = NULL;
        }
        else
        {
            printf("Failed to allocate display_config!\n");
            ret = APP_FAILURE;
        }
    }

    return ret;
}