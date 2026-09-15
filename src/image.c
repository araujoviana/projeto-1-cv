//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "image.h"
#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <math.h>

static SDL_Surface *surfaceFilter = NULL;
static SDL_Cursor *defaultMouseCursor = NULL;
static SDL_Cursor *hourglassMouseCursor = NULL;


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Image_set_bounds(Image *image, float x, float y, float width, float height)
{
    if (!image) return;
    image->rect.x = x;
    image->rect.y = y;
    image->rect.w = width;
    image->rect.h = height;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

static bool validar_extensao(const char *file)
{
    const char *extensao = strrchr(file, '.');
	
    if (extensao == NULL)
        return false;

    if (strcmp(extensao, ".jpg") == 0 ||
        strcmp(extensao, ".jpeg") == 0 ||
		strcmp(extensao, ".bmp") == 0 ||
		strcmp(extensao, ".tiff") == 0 ||
		strcmp(extensao, ".tif") == 0 ||
        strcmp(extensao, ".png") == 0)
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Image_convert(Image* image, SDL_Renderer *renderer)
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
	
	if (!surfaceFilter)
	{
		surfaceFilter = SDL_CreateSurface(image->surface->w, image->surface->h, image->surface->format);
		if (!surfaceFilter)
		{
		  SDL_Log("*** Erro: Superfície extra (filter) inválida!");
		  SDL_Log("<<< Image_convert");
		  return false;
		}
	}	

	SDL_Log("\tExecutando analise da imagem");
	SDL_SetCursor(hourglassMouseCursor);

	SDL_LockSurface(image->surface);
	SDL_LockSurface(surfaceFilter);

	const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(image->surface->format);
	Uint32 *pixels = (Uint32 *)image->surface->pixels;
	Uint32 *output = (Uint32 *)surfaceFilter->pixels;
	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	Uint8 y = 0;
	bool escala_cinza = true;

	for (int row = 0; row < image->surface->h; ++row)
	{
	for (int col = 0; col < image->surface->w; ++col)
	{
		int index = row * image->surface->w + col;
		SDL_GetRGB(pixels[index], format, NULL, &r, &g, &b);
		if ((r == g)&&(g == b))
		{
			output[index]=pixels[index];
		} 
		else 
		{
			escala_cinza = false;
			y = 0.2125*r + 0.7154*g + 0.0721*b;
			output[index] = SDL_MapRGB(format, NULL, (Uint8) y, (Uint8) y, (Uint8) y);
		}
	}
	}  

	SDL_UnlockSurface(surfaceFilter);
	SDL_UnlockSurface(image->surface);

	Image_update_texture_with_surface(image, renderer, surfaceFilter);

	if (!escala_cinza)
	{
		SDL_Log("\tConversão de imagem colorida para escala de cinza: finalizado...");
	}
	else
	{
		SDL_Log("\tImagem já está em escala de cinza...");
	}
	SDL_SetCursor(defaultMouseCursor);

	SDL_Log("<<< Image_convert");
	return true;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool Image_update_texture_with_surface(Image* image, SDL_Renderer *renderer, SDL_Surface *surface)
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
	/*
    image->rect.x = 0.0f;
    image->rect.y = 0.0f;
    image->rect.w = (float)surface->w;
    image->rect.h = (float)surface->h;
	*/
    SDL_Log("\tRect da imagem: %.0fx%.0f",image->rect.w,image->rect.h);
	
	
	SDL_Log("<<< Image_update_texture_with_surface()");
	return true;
}

//------------------------------------------------------------------------------
//
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

	SDL_Log("\tRedefinindo Image->rect...");
	image->rect.x = image->rect.y = image->rect.w = image->rect.h = 0.0f;

	SDL_Log("<<< Image_destroy()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

bool Image_load(const char *filename, SDL_Renderer *renderer, Image *output_image)
{
	SDL_Log(">>> Image_load(\"%s\")", filename);

	if (!filename)
	{
		SDL_Log("\t*** Erro: Nome do arquivo inválido (filename == NULL).");
		SDL_Log("<<< Image_load(\"%s\")", filename);
		return false;
	}
	if(!validar_extensao(filename)) {
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
//
//------------------------------------------------------------------------------

void Image_calculate_statistics(Image* image, unsigned int histogram[256], float* mean, float* std_dev)
{
    SDL_Log(">>> Image_calculate_statistics()");
    if (!image) return;
    
    SDL_Surface* target = surfaceFilter ? surfaceFilter : image->surface;
    if (!target) return;
    
    SDL_LockSurface(target);
    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(target->format);
    Uint32 *pixels = (Uint32 *)target->pixels;
    
    double sum = 0.0;
    int count = target->w * target->h;
    
    for (int i = 0; i < count; ++i) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], format, NULL, &r, &g, &b);
        Uint8 lum = (r == g && g == b) ? r : (Uint8)(0.2125*r + 0.7154*g + 0.0721*b);
        histogram[lum]++;
        sum += lum;
    }
    
    *mean = (float)(sum / count);
    
    double sum_sq_diff = 0.0;
    for (int i = 0; i < count; ++i) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], format, NULL, &r, &g, &b);
        Uint8 lum = (r == g && g == b) ? r : (Uint8)(0.2125*r + 0.7154*g + 0.0721*b);
        float diff = lum - *mean;
        sum_sq_diff += diff * diff;
    }
    *std_dev = (float)sqrt(sum_sq_diff / count);
    
    SDL_UnlockSurface(target);
    
    const char* brilho = (*mean > 127.5f) ? "Clara" : "Escura";
    const char* contraste = (*std_dev > 50.0f) ? "Alto" : "Baixo";
    
    SDL_Log("--- Estatisticas do Histograma ---");
    SDL_Log("Media de Intensidade: %.2f (Imagem %s)", *mean, brilho);
    SDL_Log("Desvio Padrao: %.2f (Contraste %s)", *std_dev, contraste);
    SDL_Log("----------------------------------");
    SDL_Log("<<< Image_calculate_statistics()");
}