# projeto-1-cv

Projeto 1 da disciplina de **Computação Visual** (UPM FCI CC) — 2026.2.

Aplicação em C (C23) usando **SDL3** para renderização.

## Dependências

- `gcc` (ou outro compilador C com suporte a C23)
- `make`
- **SDL3** (com `pkg-config` configurado)

No CachyOS / Arch:

```sh
sudo pacman -S base-devel sdl3
```

## Build

```sh
make            # compila em ./projeto-1-cv
make run        # compila e executa
make DEBUG=1    # build com símbolos + AddressSanitizer/UBSan
make clean      # remove build/ e o executável
```

Os objetos e arquivos de dependência ficam em `build/`.

## Estrutura

```
.
├── Makefile
├── src/
│   └── main.c     # ponto de entrada: abre uma janela SDL3 vazia
├── LICENSE
└── README.md
```

## Licença

[MIT](LICENSE) © 2026 Matheus Araujo
