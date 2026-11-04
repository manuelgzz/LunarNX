# Registro de desarrollo y continuidad

## 2026-11-03 — Búsqueda LAN automática más robusta (Android/Switch)

- Reporte: en Android la búsqueda automática no se ejecutaba (Linux sí). Causa
  no confirmada (no hay log del dispositivo). Cambio: un worker espera 400 ms,
  lanza `startLanDiscovery()` en el hilo de UI y, si 4 s después no hay
  consolas locales, la repite una vez; registra `Auto Search LAN attempt=N`.
  Aplica igual a Switch (misma ruta de código; revisado: nifm y difusión
  dirigida ya existían). Sin validar en dispositivo: si sigue fallando en
  Android, compartir `lunarnx.log` (líneas `ui-ps` y `ps-manager`).

## 2026-11-03 — Firma estable del APK de desarrollo

- Generada una clave PKCS12 de desarrollo (RSA 3072, ~30 años) fuera del repo y
  guardada como secretos `ANDROID_KEYSTORE_BASE64` y `ANDROID_KEYSTORE_PASSWORD`.
  Copia de respaldo local del usuario en `C:\Users\manue\LunarNX-signing\`
  (no versionar).
- `android/app/build.gradle` firma `debug` con esa clave si existe
  `LUNARNX_KEYSTORE_PATH`; el workflow la decodifica desde los secretos y,
  sin secretos (forks), conserva la clave debug de Gradle. Se imprime el
  certificado con `apksigner --print-certs`.
- Límite: el primer APK con esta clave aún exige desinstalar el anterior; los
  siguientes se actualizan sin perder configuración. CI del PR #4 (65b87bb):
  Linux 41730421568, Android 41730421547 y Switch 41730421485 en success; el
  APK quedó firmado con el certificado SHA-256
  `1b6e5bf0…09068` (clave estable). Pendiente verificar en el dispositivo la
  instalación encima de una compilación posterior con esa misma clave.

## 2026-11-03 — Búsqueda LAN automática al abrir PlayStation

- Rama `manuelgzz-ps-auto-search-lan` sobre `develop` (c1cd30f).
- Al abrir la página PlayStation se lanza en segundo plano la misma acción del
  botón "Search LAN" (`PsActivity::startLanDiscovery`, diferida con
  `brls::sync`). No valida que la consola esté encendida ni bloquea la interfaz;
  el botón sigue disponible. Código compartido: aplica a Switch, Linux y Android.
  No se dispara al reanudar la página ni se lanza el refresco PSN.
- Validación: `tests/ps_page_flow_test.py` actualizado (antes exigía no
  descubrir al abrir) y pruebas PS/UI enfocadas pasan; `git diff --check` limpio.
  No se compiló Switch/Linux/Android localmente: pendiente CI.
- Pruebas del usuario en Android (reportadas, sin registro formal): Xbox más
  estable con decodificación por software; PlayStation mejor con MediaCodec;
  gamepads táctiles correctos.
- Problema abierto: en Linux con PlayStation el indicador siempre muestra SW,
  incluso reiniciando la sesión; sin determinar si el cambio de codec es real o
  solo falla el indicador. Revisión de código: el decodificador PS comparte la
  ruta de aceleración con Xbox (`tryNativeAcceleration`) y el indicador solo
  cambia tras entregar un cuadro nativo; no se halló un fallo evidente. Se
  añadieron registros `[video-decode]` (`accelerator-attempt`, códec, decoder,
  texto de error FFmpeg) para identificar el motivo. Requiere el log del
  usuario: `~/.cache/lunarnx/lunarnx.log`. Sospechas sin confirmar: libva/
  controlador del AppImage o FFmpeg sin VA-API. Sin compilar localmente.
- Sin validación en hardware de la búsqueda automática.
## 2026-11-02 — Gamepads táctiles Xbox/PlayStation en Linux y Android

- Continuación del PR #3, rama feature/native-decoder-selection. Añadido
  TouchGamepad con geometría compartida entre entrada y dibujo, asignación de
  dedos hasta su liberación, sticks analógicos, cruceta/diagonales, botones,
  gatillos y zona touchpad PS. El mando virtual se combina después del mapeo
  físico para que las etiquetas indiquen directamente el botón remoto.
- Ajustes independientes Xbox/PS de visibilidad y opacidad 10–100%; oculto/50%
  de inicio. Desde el menú `...`, guardar los ajustes y volver aplica cambios
  durante la sesión. El menú sigue accesible sin un mando físico ni gamepad.
- Con gamepad visible, solo la zona PS central produce contactos, escalados al
  touchpad remoto. Al ocultarlo vuelve el touchpad de pantalla completa. No se
  cambia el touchpad de un mando físico. Menús, foco, cambios de modo y salida
  limpian contactos/botones y evitan reutilizar dedos mantenidos sobre la UI.
- La ruta Switch se conserva. No se implementan captura Xbox ni micrófono/mute
  DualSense: no tienen una ruta de entrada/audio disponible en esta app.
- Pruebas: build Linux nativo en Docker Ubuntu 24.04; CTest nativo 4/4, portable
  12/12 PASS. touch_gamepad cubre botones, ambos sticks, combinaciones con dos
  contactos PS, límites del pad, separación de dedos, menú/foco y cambio de modo.
  La tolerancia de clic/gestos sigue en píxeles originales para no amplificar el
  pequeño temblor del dedo al escalar la zona dedicada a coordenadas de consola.
  El overlay ocupa el viewport completo, incluyendo tablets no 16:9, para
  alinear las zonas táctiles normalizadas con los controles dibujados; revisión
  visual y replay al redimensionar la ventana a 1280x800: PASS.
  Persistencia Xbox/PS y coexistencia con mando SDL en native_ui_settings.
- Smoke local de navegación y replay PS H.264 a 720p con gamepad visible: PASS.
  Regresiones de ciclo de vida/cierre UI, navegación y orden/cancelación Xbox,
  junto con git diff --check: PASS. Revisión visual del overlay con captura de
  framebuffer; se usa la fuente registrada por Borealis para las etiquetas.
- CI añade gamepad Xbox visible al mock nativo y PS visible en replay H.264,
  conservando PS fullscreen en HEVC. No se ejecutó Ryubing localmente: la
  cobertura nueva es la ruta nativa; la compilación NRO se comprueba en CI.
  Nuevos APK/NRO y pruebas físicas multitáctiles permanecen pendientes.
- Siguiente paso: probar simultáneamente sticks, gatillos y botones, transiciones
  menú/juego, transparencia y gesto/clic PS con gamepad visible y oculto.

## 2026-11-02 — Selección de decoder e indicador Linux/Android

- Rama `feature/native-decoder-selection`, basada en develop. Xbox (consola y
  Cloud) y PlayStation guardan modos Automático/Software independientes y los
  transmiten hasta VideoDecoder mediante MediaPipelineOptions. Automático
  conserva el intento de aceleración y recuperación por CPU; Software evita
  el intento. El cambio se aplica en la siguiente sesión.
- Indicador opcional del decoder que produce imagen (VA-API/MediaCodec/SW),
  con interruptor persistente en ajustes generales y menú durante el juego.
  Funciona sin activar las estadísticas; comienza oculto. Menú nativo ajustado
  para alojar el interruptor. Antes de recibir cuadros indica `...`.
- Sustituido el selector nativo que siempre mostraba Software aunque se pudiera
  activar aceleración. Se conservan los modos de Switch y el override por entorno.
- Añadidas pruebas de política, persistencia independiente y smoke de CPU por
  preferencia, manteniendo la prueba de recuperación del acelerador en CI.
- Pruebas reales reportadas por el usuario para la compilación anterior: 720p,
  consolas cableadas y clientes Wi-Fi; PS Android muy estable; Xbox Android con
  pausas muy ocasionales; PS Linux con pausas ocasionales; Xbox Linux con pausas
  frecuentes. No se conoce el SHA exacto ni el decoder activo de esas pruebas.
- Validación local en Ubuntu 24.04 dentro de Docker: compilación completa
  `cmake -S linux/ui -B build/linux-ui -DCMAKE_BUILD_TYPE=Release` y
  `cmake --build build/linux-ui --parallel 4`: PASS. `ctest --test-dir
  build/linux-ui --output-on-failure`: 4/4 PASS (incluye persistencia Xbox/PS,
  modo CPU y visibilidad del indicador). CTest portable: 11/11 PASS.
- `native_video_decode_smoke` con fixtures H.264/HEVC: PASS para CPU elegida por
  ajustes (`LUNARNX_TEST_SETTINGS_SOFTWARE=1`), VA-API no disponible,
  override CPU y recuperación en ejecución. Cada caso decodificó y presentó
  8 cuadros Xbox H.264, PS H.264 y PS HEVC, con prueba de audio Opus.
- `LunarNX_Linux --ui-smoke`: PASS para ajustes generales, Xbox y PS;
  `--ps-replay` H.264 a 720p con indicador habilitado: PASS, 10 cuadros
  presentados y 42 unidades de audio, una sola ventana, salida limpia.
- Regresiones de orden/cancelación Xbox, ciclo de vida/cierre UI, navegación,
  workflows Switch e idioma xCloud, JSON de traducciones y `git diff --check`:
  PASS. Sin pruebas nuevas en tablet, ROG Ally o Switch físico.
- No se compilaron APK Android ni NRO Switch; tampoco se ejecutó Ryubing.
  La cobertura de reproducción de este cambio nativo es el smoke Linux anterior.
  Los workflows existentes pueden validar los tres destinos al abrir un PR.
- Publicación autorizada por el usuario: rama publicada y
  [PR #3 hacia develop](https://github.com/manuelgzz/LunarNX/pull/3) abierto.
  Los workflows Linux, Android y Switch se disparan desde el PR; consultar
  [Actions de la rama](https://github.com/manuelgzz/LunarNX/actions?query=branch%3Afeature%2Fnative-decoder-selection)
  para resultado, SHA y disponibilidad de cada artefacto. La validación local
  anterior corresponde al cambio de código d7dfb9a; este seguimiento es
  exclusivamente documental y no modifica binarios.
- Siguiente paso: comparar modos con indicador visible y registrar commit,
  decoder efectivo, fluidez, audio y controles, priorizando Xbox/Linux.

## 2026-11-02 — Consolidación en develop

- Integración con historial del último port, main y estabilidad/diagnósticos.
- Conservada la corrección de liberación CShader antes de CMemPool y las
  opciones DROP_DIAG/LATENCY_DIAG de Switch.
- Los tres workflows compilan develop; main conserva su línea estable.
- Guías y AGENTS.md apuntan ahora a develop. Se registra origen y procedimiento
  en docs/branch-consolidation.md.
- Estado de consolidación: completada; CI Linux, Android y Switch exitosa y ramas anteriores retiradas.
- Validación final y enlaces: docs/consolidation-validation.md. El cierre del
  PR #2 y retirada de ramas se ejecutan solo después de tres builds exitosos,
  comprobación de SHA y de ancestros. Hardware sigue pendiente.

## 2026-11-02 — Documentación para retomar desde otro equipo

- Añadido `docs/CONTINUAR_DESARROLLO.md` con rama activa, estado, pruebas,
  artefactos, pendientes y procedimiento para otra PC o Codex Cloud.
- Añadida a AGENTS.md la obligación de leer el estado al iniciar y actualizar
  el estado, este registro y las guías afectadas al terminar cada cambio.
- Añadido acceso desde README. En la rama del port se corrigen notas obsoletas
  sobre aceleración, interfaz y empaquetado.
- Base de código revisada: `59a3efec43854cf5ad3766af0fbb77f673a1cac2`.
  Linux Actions 41537165614 y Android Actions 41537165550: success.
- Validación de este cambio: revisión de archivos, enlaces relativos y espacios
  en blanco. No se recompilan binarios por cambios exclusivamente documentales.
- Sin validación nueva en hardware; no se creó un entorno Cloud ni se auditó
  trabajo sin subir en otras máquinas.
- Siguiente paso: pruebas de tablet/ROG Ally descritas en el estado; preparar y
  publicar un entorno Cloud si se quiere independencia de esta PC.

## Referencias de implementación anteriores

Estas entradas se reconstruyen desde commits y CI, no son pruebas nuevas:

- `8d6b4b9`: VA-API/MediaCodec con recuperación por CPU.
- `dcaddc9`, `f83043c`, `91397c8`: cobertura de recuperación y codec,
  separación del programa de prueba y disparadores CI.
- `69e37ad`, `59a3efe`: contactos/clics táctiles PlayStation y liberación de
  contactos antes de reutilizar ranuras Chiaki.
- `72144c0`: corrección de arranque Android relacionada con JNI de rumble.
- `f96469f`: confianza TLS nativa y diagnóstico de fallos de acceso Xbox.
- `6fbcafd`: conservación de cuadros inmutables entre repintados.

## Formato para siguientes entradas

Fecha y cambio; rama y base de código; qué cambió y por qué; validación
(comandos, resultados y enlace CI cuando exista); límites y riesgos;
pendientes y siguiente paso. No presentar compilación o simulación como
confirmación de funcionamiento en hardware real.
