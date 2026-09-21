// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// Copyright (c) 2026 Matheus Gabriel Viana Araujo, Eduardo Takashi Missaka,
//                    Arthur Meneses Neves, João Victor Vidal Barbosa
// SPDX-License-Identifier: Apache-2.0
// Derivado de https://github.com/profkishimoto/CompVis262. Modificado pelo grupo.

#ifndef IMAGE_H
#define IMAGE_H

//=================================================================================================

#include <SDL3/SDL.h>
#include <stdbool.h>

//=================================================================================================

typedef struct Image Image;
struct Image
{
  SDL_Surface *surface;
  SDL_Surface *gray;
  SDL_Surface *equalized;
  SDL_Surface *current; // aponta para uma das outras, nao e dona da memoria
  SDL_Texture *texture;
  SDL_FRect rect;
};

//=================================================================================================

bool Image_update_texture_with_surface(Image *image, SDL_Renderer *renderer, SDL_Surface *surface);
bool Image_load(const char *filename, SDL_Renderer *renderer, Image *output_image);
bool Image_convert(Image *image, SDL_Renderer *renderer);
bool Image_equalize(Image *image, SDL_Renderer *renderer);
bool Image_show_original(Image *image, SDL_Renderer *renderer);
bool Image_save_current(const Image *image, const char *filename, int width, int height);

void Image_destroy(Image *image);
void Image_set_bounds(Image *image, float x, float y, float width, float height);
void Image_calculate_statistics(Image *image, unsigned int histogram[256], float *mean,
                                float *std_dev);

//=================================================================================================

#endif