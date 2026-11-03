# Registro de desarrollo y continuidad

## 2026-11-02 — Consolidación en develop

- Integración con historial del último port, main y estabilidad/diagnósticos.
- Conservada la corrección de liberación CShader antes de CMemPool y las
  opciones DROP_DIAG/LATENCY_DIAG de Switch.
- Los tres workflows compilan develop; main conserva su línea estable.
- Guías y AGENTS.md apuntan ahora a develop. Se registra origen y procedimiento
  en docs/branch-consolidation.md.
- Estado de consolidación: pendiente de validación CI y retirada de ramas.
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
