# Pong

El clásico de tenis de mesa: a 2 jugadores o contra la CPU. Desarrollado con **Qt6** y **Deepin Tool Kit (DTK6)** para Deepin Linux.

## Características

- Modo de 1 jugador contra la CPU (menú) o 2 jugadores en el mismo teclado
- Partidas a 11 puntos con pantalla de ganador
- La pelota acelera con cada golpe y el ángulo depende de dónde golpea
- Pausa automática al cambiar de ventana
- Traducido a 8 idiomas

## Controles

| Tecla | Acción |
|-------|--------|
| W / S | Pala izquierda |
| ↑ / ↓ | Pala derecha (o la tuya contra la CPU) |
| P / Esc | Pausa |
| Enter | Jugar otra vez |
| Ctrl+N | Nuevo juego |
| Ctrl+Q | Salir |

## Instalación

Descarga el paquete de la [última release](https://github.com/deepin-espanol/pong/releases/latest):

- `.deb` (amd64, arm64, riscv64, loong64):
  ```bash
  sudo apt install ./pong_*.deb
  ```
- `.uab` de linyaps (amd64, arm64): ejecutable directamente.
  ```bash
  chmod +x org.deepinenespanol.*.uab && ./org.deepinenespanol.*.uab
  ```

## Compilar

```bash
sudo apt build-dep ./          # dependencias declaradas en debian/control
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/pong
```

O generar el paquete `.deb`:

```bash
dpkg-buildpackage -us -uc -b
```

## Publicar una versión

Añade una entrada nueva en `debian/changelog` y haz push: el workflow
*Build and publish .deb* crea la release con los `.deb` y los `.uab`.

## Licencia

GPL-3.0 o posterior. © Deepin en Español — https://deepinenespanol.org
