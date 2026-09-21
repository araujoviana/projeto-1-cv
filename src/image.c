// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// Copyright (c) 2026 Matheus Gabriel Viana Araujo, Eduardo Takashi Missaka,
//                    Arthur Meneses Neves, João Victor Vidal Barbosa
// SPDX-License-Identifier: Apache-2.0
// Derivado de https://github.com/profkishimoto/CompVis262. Modificado pelo grupo.

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "image.h"
#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


//------------------------------------------------------------------------------

// Pixel ja cinza passa direto: a formula truncada tira 1 nivel de alguns tons (ex.: 43).
static Uint8 pixel_luminance(Uint8 r, Uint8 g, Uint8 b)
{
  if (r == g && g == b)
    return r;
  return (Uint8)(0.2125 * r + 0.7154 * g + 0.0721 * b);
}

//------------------------------------------------------------------------------

void Image_set_bounds(Image *image, float x, float y, float width, float height)
{
  if (!image)
    return;
  image->rect.x = x;
  image->rect.y = y;
  image->rect.w = width;
  image->rect.h = height;
}

//------------------------------------------------------------------------------

static bool validar_extensao(const char *file)
{
  const char *extensao = strrchr(file, '.');

  if (extensao == NULL)
    return false;

  if (strcmp(extensao, ".jpg") == 0 || strcmp(extensao, ".jpeg") == 0 ||
      strcmp(extensao, ".bmp") == 0 || strcmp(extensao, ".tiff") == 0 ||
      strcmp(extensao, ".tif") == 0 || strcmp(extensao, ".png") == 0)
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Image_convert(Image *image, SDL_Renderer *renderer)
{
  SDL_Log(">>> Image_convert");

  if (!image || !image->surface)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL ou image->surface == NULL).");
    SDL_Log("<<< Image_convert");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< Image_convert");
    return false;
  }

  if (!image->gray)
  {
    image->gray = SDL_CreateSurface(image->surface->w, image->surface->h, image->surface->format);
    if (!image->gray)
    {
      SDL_Log("*** Erro: Superfície extra (filter) inválida!");
      SDL_Log("<<< Image_convert");
      return false;
    }
  }

  SDL_Log("\tExecutando analise da imagem");

  SDL_LockSurface(image->surface);
  SDL_LockSurface(image->gray);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->surface->format);
  Uint32 *pixels = (Uint32 *)image->surface->pixels;
  Uint32 *output = (Uint32 *)image->gray->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  bool escala_cinza = true;

  for (int row = 0; row < image->surface->h; ++row)
  {
    for (int col = 0; col < image->surface->w; ++col)
    {
      int index = row * image->surface->w + col;
      SDL_GetRGB(pixels[index], format, NULL, &r, &g, &b);
      if ((r == g) && (g == b))
      {
        output[index] = pixels[index];
      }
      else
      {
        escala_cinza = false;
        Uint8 y = pixel_luminance(r, g, b);
        output[index] = SDL_MapRGB(format, NULL, y, y, y);
      }
    }
  }

  SDL_UnlockSurface(image->gray);
  SDL_UnlockSurface(image->surface);

  if (!Image_update_texture_with_surface(image, renderer, image->gray))
  {
    SDL_Log("<<< Image_convert");
    return false;
  }
  image->current = image->gray;

  if (!escala_cinza)
  {
    SDL_Log("\tConversão de imagem colorida para escala de cinza: finalizado...");
  }
  else
  {
    SDL_Log("\tImagem já está em escala de cinza...");
  }

  SDL_Log("<<< Image_convert");
  return true;
}

//------------------------------------------------------------------------------
bool Image_update_texture_with_surface(Image *image, SDL_Renderer *renderer, SDL_Surface *surface)
{
  SDL_Log(">>> Image_update_texture_with_surface()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< Image_update_texture_with_surface()");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< Image_update_texture_with_surface()");
    return false;
  }

  if (!surface)
  {
    SDL_Log("\t*** Erro: Superfície inválida (surface == NULL).");
    SDL_Log("<<< Image_update_texture_with_surface()");
    return false;
  }

  SDL_DestroyTexture(image->texture);

  image->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!image->texture)
  {
    SDL_Log("\t*** Erro ao criar textura: %s", SDL_GetError());
    SDL_Log("<<< Image_update_texture_with_surface()");
    return false;
  }
  SDL_Log("\tRect da imagem: %.0fx%.0f", image->rect.w, image->rect.h);

  SDL_Log("<<< Image_update_texture_with_surface()");
  return true;
}

//------------------------------------------------------------------------------

bool Image_equalize(Image *image, SDL_Renderer *renderer)
{
  SDL_Log(">>> Image_equalize()");

  if (!image || !image->surface)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL ou image->surface == NULL).");
    SDL_Log("<<< Image_equalize()");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< Image_equalize()");
    return false;
  }

  if (!image->gray)
  {
    SDL_Log("\t*** Erro: Superfície base (image->gray) ausente.");
    SDL_Log("<<< Image_equalize()");
    return false;
  }

  if (!image->equalized)
  {
    image->equalized = SDL_CreateSurface(image->gray->w, image->gray->h, image->gray->format);
    if (!image->equalized)
    {
      SDL_Log("\t*** Erro ao criar superfície equalizada: %s", SDL_GetError());
      SDL_Log("<<< Image_equalize()");
      return false;
    }
  }

  int count = image->gray->w * image->gray->h;
  if (count <= 0)
  {
    SDL_Log("\t*** Erro: Dimensões da imagem inválidas.");
    SDL_Log("<<< Image_equalize()");
    return false;
  }

  SDL_LockSurface(image->gray);
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->gray->format);
  const Uint32 *in_pixels = (const Uint32 *)image->gray->pixels;

  unsigned int hist[256] = {0};
  for (int i = 0; i < count; ++i)
  {
    Uint8 r, g, b;
    SDL_GetRGB(in_pixels[i], format, NULL, &r, &g, &b);
    hist[r]++;
  }

  // s_k = round(255 * CDF(k) / N)
  Uint8 lut[256];
  unsigned long long cum = 0;
  for (int k = 0; k < 256; ++k)
  {
    cum += hist[k];
    double sk = round(255.0 * (double)cum / (double)count);
    if (sk < 0.0)
    {
      sk = 0.0;
    }
    else if (sk > 255.0)
    {
      sk = 255.0;
    }
    lut[k] = (Uint8)sk;
  }

  SDL_LockSurface(image->equalized);
  Uint32 *out_pixels = (Uint32 *)image->equalized->pixels;

  for (int i = 0; i < count; ++i)
  {
    Uint8 r, g, b, a;
    SDL_GetRGBA(in_pixels[i], format, NULL, &r, &g, &b, &a);
    Uint8 eq_val = lut[r];
    out_pixels[i] = SDL_MapRGBA(format, NULL, eq_val, eq_val, eq_val, a);
  }

  SDL_UnlockSurface(image->equalized);
  SDL_UnlockSurface(image->gray);

  if (!Image_update_texture_with_surface(image, renderer, image->equalized))
  {
    SDL_Log("<<< Image_equalize()");
    return false;
  }
  image->current = image->equalized;

  SDL_Log("\tEqualização de histograma concluída com sucesso.");
  SDL_Log("<<< Image_equalize()");
  return true;
}

//------------------------------------------------------------------------------

bool Image_show_original(Image *image, SDL_Renderer *renderer)
{
  SDL_Log(">>> Image_show_original()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< Image_show_original()");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< Image_show_original()");
    return false;
  }

  if (!image->gray)
  {
    SDL_Log("\t*** Erro: Superfície original ausente.");
    SDL_Log("<<< Image_show_original()");
    return false;
  }

  bool result = Image_update_texture_with_surface(image, renderer, image->gray);
  if (result)
  {
    image->current = image->gray;
    SDL_Log("\tExibindo imagem em escala de cinza original (sem reabrir arquivo).");
  }

  SDL_Log("<<< Image_show_original()");
  return result;
}

//------------------------------------------------------------------------------

bool Image_save_current(const Image *image, const char *filename, int width, int height)
{
  if (!image || !filename || !image->current)
  {
    SDL_Log("Erro ao salvar imagem: nenhuma imagem esta sendo exibida.");
    return false;
  }

  bool overwritten = false;
  FILE *existing_file = fopen(filename, "rb");
  if (existing_file)
  {
    overwritten = true;
    fclose(existing_file);
  }

  // a janela pode estar mostrando a imagem em outro tamanho que o original
  SDL_Surface *to_save = image->current;
  bool scaled =
      (width > 0 && height > 0 && (width != image->current->w || height != image->current->h));
  if (scaled)
  {
    to_save = SDL_ScaleSurface(image->current, width, height, SDL_SCALEMODE_LINEAR);
    if (!to_save)
    {
      SDL_Log("Erro ao redimensionar a imagem para salvar: %s", SDL_GetError());
      return false;
    }
  }

  bool saved = IMG_Save(to_save, filename);
  if (scaled)
  {
    SDL_DestroySurface(to_save);
  }
  if (!saved)
  {
    SDL_Log("Erro ao salvar %s: %s", filename, SDL_GetError());
    return false;
  }

  SDL_Log("arquivo %s %s", filename, overwritten ? "sobrescrito" : "criado");
  return true;
}

//------------------------------------------------------------------------------

void Image_destroy(Image *image)
{
  SDL_Log(">>> Image_destroy()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< Image_destroy()");
    return;
  }

  if (image->texture)
  {
    SDL_Log("\tDestruindo Image->texture...");
    SDL_DestroyTexture(image->texture);
    image->texture = NULL;
  }

  if (image->surface)
  {
    SDL_Log("\tDestruindo Image->surface...");
    SDL_DestroySurface(image->surface);
    image->surface = NULL;
  }

  if (image->equalized)
  {
    SDL_Log("\tDestruindo Image->equalized...");
    SDL_DestroySurface(image->equalized);
    image->equalized = NULL;
  }

  if (image->gray)
  {
    SDL_Log("\tDestruindo Image->gray...");
    SDL_DestroySurface(image->gray);
    image->gray = NULL;
  }
  image->current = NULL;

  SDL_Log("\tRedefinindo Image->rect...");
  image->rect.x = image->rect.y = image->rect.w = image->rect.h = 0.0f;

  SDL_Log("<<< Image_destroy()");
}

//------------------------------------------------------------------------------

bool Image_load(const char *filename, SDL_Renderer *renderer, Image *output_image)
{
  SDL_Log(">>> Image_load(\"%s\")", filename);

  if (!filename)
  {
    SDL_Log("\t*** Erro: Nome do arquivo inválido (filename == NULL).");
    SDL_Log("<<< Image_load((null))");
    return false;
  }
  if (!validar_extensao(filename))
  {
    SDL_Log("\t*** Erro: Extensão do arquivo inválida (Não é formato de imagem).");
    SDL_Log("<<< Image_load(\"%s\")", filename);
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< Image_load(\"%s\")", filename);
    return false;
  }

  if (!output_image)
  {
    SDL_Log("\t*** Erro: Imagem de saída inválida (output_image == NULL).");
    SDL_Log("<<< Image_load(\"%s\")", filename);
    return false;
  }

  Image_destroy(output_image);

  SDL_Log("\tCarregando imagem \"%s\" em uma superfície...", filename);
  SDL_Surface *surface = IMG_Load(filename);
  if (!surface)
  {
    SDL_Log("\t*** Erro ao carregar a imagem: %s", SDL_GetError());
    SDL_Log("<<< Image_load(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tConvertendo superfície para formato RGBA32...");
  output_image->surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(surface);
  if (!output_image->surface)
  {
    SDL_Log("\t*** Erro ao converter superfície para formato RGBA32: %s", SDL_GetError());
    SDL_Log("<<< load(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tCriando textura a partir da superfície...");
  if (!Image_update_texture_with_surface(output_image, renderer, output_image->surface))
  {
    SDL_Log("\t*** Erro ao criar textura.");
    SDL_Log("<<< Image_load(\"%s\")", filename);
    return false;
  }

  SDL_Log("<<< Image_load(\"%s\")", filename);
  return true;
}

//------------------------------------------------------------------------------

void Image_calculate_statistics(Image *image, unsigned int histogram[256], float *mean,
                                float *std_dev)
{
  SDL_Log(">>> Image_calculate_statistics()");
  if (!image)
    return;

  memset(histogram, 0, 256 * sizeof(unsigned int));

  SDL_Surface *target =
      image->current ? image->current : (image->gray ? image->gray : image->surface);
  if (!target)
    return;

  SDL_LockSurface(target);
  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(target->format);
  Uint32 *pixels = (Uint32 *)target->pixels;

  double sum = 0.0;
  int count = target->w * target->h;

  for (int i = 0; i < count; ++i)
  {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], format, NULL, &r, &g, &b);
    Uint8 lum = pixel_luminance(r, g, b);
    histogram[lum]++;
    sum += lum;
  }

  *mean = (float)(sum / count);

  double sum_sq_diff = 0.0;
  for (int i = 0; i < count; ++i)
  {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], format, NULL, &r, &g, &b);
    Uint8 lum = pixel_luminance(r, g, b);
    float diff = lum - *mean;
    sum_sq_diff += diff * diff;
  }
  *std_dev = (float)sqrt(sum_sq_diff / count);

  SDL_UnlockSurface(target);

  // limiares: um terco da faixa [0, 255] para a media; desvio 30/60 e escolha nossa
  const char *brilho = (*mean < 85.0f) ? "Escura" : ((*mean <= 170.0f) ? "Média" : "Clara");
  const char *contraste = (*std_dev < 30.0f) ? "Baixo" : ((*std_dev <= 60.0f) ? "Médio" : "Alto");

  SDL_Log("--- Estatisticas do Histograma ---");
  SDL_Log("Media de Intensidade: %.2f (Imagem %s)", *mean, brilho);
  SDL_Log("Desvio Padrao: %.2f (Contraste %s)", *std_dev, contraste);
  SDL_Log("----------------------------------");
  SDL_Log("<<< Image_calculate_statistics()");
}