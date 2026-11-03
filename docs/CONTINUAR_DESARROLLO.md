# Continuar DualStream / LunarNX desde otra sesión

Última actualización: 2026-11-02 (America/Chihuahua).
Repositorio de trabajo: https://github.com/manuelgzz/LunarNX

## Leer primero

El desarrollo nativo Linux/Android está en **port/ps-touchscreen-linux-android**.
No asumir que `main` contiene ese trabajo. Esta guía también se publica en
`main` para encontrar la rama correcta; el estado vigente debe consultarse en
la rama de trabajo. `port/linux-native-foundation` conserva la base anterior.

Leer en este orden: `AGENTS.md`, este documento,
[registro de cambios](development-log.md), [empaquetado](https://github.com/manuelgzz/LunarNX/blob/port/ps-touchscreen-linux-android/docs/packaging.md),
[Linux](https://github.com/manuelgzz/LunarNX/blob/port/ps-touchscreen-linux-android/docs/linux-development.md) y [dirección del port](https://github.com/manuelgzz/LunarNX/blob/port/ps-touchscreen-linux-android/docs/linux-android-port.md).
Los tres últimos documentos están en la rama del port.

DualStream es el nombre usado en las conversaciones para el objetivo de unificar
Xbox y PlayStation. El repositorio, ejecutables y paquetes todavía usan LunarNX.
El lanzador Python Dualstream es una implementación separada: no confundirlo con
este port nativo ni migrarlo sin una decisión explícita.

## Estado verificado en GitHub

Referencia de código revisada: `59a3efec43854cf5ad3766af0fbb77f673a1cac2`.
Esta referencia identifica la base de código; los commits posteriores de
documentación no significan que los binarios hayan sido recompilados.

- Interfaz Borealis propia con motores Xbox y PlayStation integrados; no abre
  Chiaki-ng ni XStreamingDesktop como aplicaciones externas.
- Paquetes de desarrollo: AppImage Linux x86_64 y APK Android ARM64.
- Linux intenta VA-API y Android MediaCodec para H.264/HEVC, con recuperación
  por CPU. Hay un intento de aceleración por sesión.
- Presentación mediante RGBA y subida a la interfaz; no es zero-copy.
- El indicador VA-API / MediaCodec / SW se actualiza al producir una imagen.
  MediaCodec no demuestra por sí solo que el dispositivo haya elegido un codec
  físico del fabricante.
- Los últimos cambios corrigen contactos, clics y reemplazo de contactos del
  touchpad PlayStation en pantallas táctiles Linux/Android.
- Se conservan los límites de memoria y las restricciones Switch de AGENTS.md.

Resultados consultados en esta actualización, ambos exitosos para esa referencia:

| Plataforma | Ejecución | Artefacto |
| --- | --- | --- |
| Linux | [41537165614](https://github.com/manuelgzz/LunarNX/actions/runs/41537165614) | LunarNX-Linux-AppImage-x86_64 |
| Android | [41537165550](https://github.com/manuelgzz/LunarNX/actions/runs/41537165550) | LunarNX-Android-ARM64-APK |

Los artefactos tienen caducidad; no son releases permanentes. Incluyen fuentes
correspondientes. Si ya no están disponibles, reconstruir desde una referencia
identificada usando los workflows existentes.

CI Linux valida pruebas portables, decodificación y recuperación por CPU,
mock Xbox, reproducción PlayStation H.264/HEVC, interfaz y AppImage.
CI Android valida compilación, enlace, firma y contrato del APK. Consultar los
pasos de cada ejecución antes de atribuirle una prueba concreta. Ninguno
demuestra fluidez o aceleración física en la tablet o ROG Ally.

## Próximo trabajo

1. Probar el AppImage en la ROG Ally con Linux y el APK en la tablet Android.
   Registrar versión/commit, sistema, plataforma Xbox/PS, codec, resolución,
   indicador VA-API/MediaCodec/SW, fluidez, audio y controles.
2. Comprobar recuperación por CPU y comparar rendimiento; Linux permite
   `LUNARNX_VIDEO_DECODE=software`. No declarar aceleración física validada
   hasta observarla en el equipo.
3. Probar touchpad PS con uno/dos dedos, clic corto, pulsación sostenida,
   reemplazo de contactos y abrir/cerrar el menú sin dejar contactos atascados.
4. Validar autenticación real, streaming LAN/PSN, reconexión y suspensión.
5. Corregir problemas reproducibles con pruebas enfocadas y actualizar estas
   notas y el registro al terminar cada cambio.

El APK es de desarrollo y puede cambiar de firma entre compilaciones. No
desinstalar para resolver una actualización sin considerar que se perderán
configuración y cuentas guardadas. No publicar credenciales para reproducirlo.

## Continuar en otra PC

```sh
git clone https://github.com/manuelgzz/LunarNX.git
cd LunarNX
git switch port/ps-touchscreen-linux-android
git status --short --branch
git log -5 --oneline
```

Si ya existe el checkout, revisar primero cambios locales; después hacer fetch
y cambiar a la rama sin sobrescribir trabajo. Preparar herramientas según los
documentos de plataforma y los workflows; clonar no instala dependencias.

Texto para iniciar una sesión nueva:

> Continúa el desarrollo de manuelgzz/LunarNX en la rama
> port/ps-touchscreen-linux-android. Lee AGENTS.md,
> docs/CONTINUAR_DESARROLLO.md y docs/development-log.md. Revisa los últimos
> commits y GitHub Actions antes de cambiar código. Retoma los pendientes
> documentados y actualiza el estado y el registro al terminar cada cambio.

El historial local de Codex no se recupera solo con nombrar el repositorio.
GitHub conserva archivos y commits publicados, no cambios locales sin subir,
herramientas instaladas ni credenciales. Estas notas reemplazan la dependencia
de una conversación para retomar el desarrollo.

## Preparar trabajo en la nube y móvil

No se ha creado ni publicado un entorno Codex Cloud como parte de esta
documentación. GitHub Actions ya compila en la nube, pero eso es distinto de
un entorno de desarrollo Codex Cloud.

Desde web/escritorio, si la función está disponible en la cuenta:

1. Crear un entorno en Work in > Cloud > Create environment.
2. Conectar GitHub, seleccionar manuelgzz/LunarNX y usar la rama del port al
   iniciar la tarea; comprobar la rama y el commit dentro del entorno.
3. Preparar dependencias siguiendo los workflows Linux/Android y sus versiones
   fijadas. Para Switch se necesita Docker/devkitA64; verificar disponibilidad.
4. Ejecutar pruebas apropiadas, revisar la configuración y publicar el entorno.
5. Abrir Codex desde un dispositivo compatible, elegir el entorno publicado y
   continuar la tarea con el texto anterior.

No asumir que el identificador de esta sesión local importa el chat a Cloud.
El código y estas notas aportan el contexto para una tarea nueva. Las pruebas
de GPU, táctil y streaming en dispositivos reales siguen necesitando hardware.

Guía oficial: https://learn.chatgpt.com/docs/environments/cloud-environments

## Mantener la continuidad

Después de cada cambio terminado, actualizar este estado y
`docs/development-log.md` en el mismo commit cuando sea posible: comportamiento,
archivos relevantes, comandos de validación y resultado, limitaciones, siguiente
paso y enlaces a commits/CI/artefactos cuando existan. Actualizar las guías
afectadas. Distinguir implementado, validado en CI y validado en hardware.
Si un dato no se verificó, decirlo. No inventar resultados ni publicar tokens,
cuentas, IP privadas, logs completos o conversaciones sin depurar.
