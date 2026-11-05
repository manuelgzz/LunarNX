# Changelog

Notable LunarNX changes are recorded here by the automated release workflow.

## [0.4.0](https://github.com/manuelgzz/LunarNX/compare/v0.3.3...v0.4.0) (2026-11-05)


### Features

* **auth:** streamline Xbox phone sign-in ([5ad45d4](https://github.com/manuelgzz/LunarNX/commit/5ad45d44ad4bd0f350c24a589543e56fe63934c6))
* **ci:** automate versioned GitHub releases ([2b609dc](https://github.com/manuelgzz/LunarNX/commit/2b609dc6ef616b9ce23c5aae26931f9a8293bea0))
* **dev-bridge:** name downloaded builds by version ([cfa635c](https://github.com/manuelgzz/LunarNX/commit/cfa635cbf1cb91532936bf9f15bd05bb000546ec))
* **dev-bridge:** unify build and publish versions ([6647a3a](https://github.com/manuelgzz/LunarNX/commit/6647a3a35a3def3cbf42401d74d2ffeefba083b3))
* **dev:** add remote build and log bridge ([19b904f](https://github.com/manuelgzz/LunarNX/commit/19b904f1225bd0b044247639172368316e845e88))
* improve streaming quality and remote connectivity ([25ec90c](https://github.com/manuelgzz/LunarNX/commit/25ec90cf1e912e3a55609903598ce304633d20d4))
* **input:** add streaming button mapping ([c34cb4b](https://github.com/manuelgzz/LunarNX/commit/c34cb4b5dfdbc2e5a78f4a17fa300ac775f0fc34))
* **ps:** add HEVC streaming support ([809e847](https://github.com/manuelgzz/LunarNX/commit/809e8476749ed76b765f3873b3e092e3a36f5006))
* **ps:** add PlayStation Remote Play support ([944272a](https://github.com/manuelgzz/LunarNX/commit/944272a93ce12894f0ec04783f709747065e36d0))
* **ps:** cache remote console discovery ([d33aff1](https://github.com/manuelgzz/LunarNX/commit/d33aff1be88bb625303dce3b0f3064c4d9df5952))
* **ps:** clarify local pairing ID formats ([d4bf7ff](https://github.com/manuelgzz/LunarNX/commit/d4bf7ff2ff72b3001bde07069a2174b27baeb32d))
* **ps:** forward Switch motion sensors ([5be920c](https://github.com/manuelgzz/LunarNX/commit/5be920c64f67fa7d0aaf58d536d1b2514c0b7e8f))
* **ps:** harden connection lifecycle and diagnostics ([0644691](https://github.com/manuelgzz/LunarNX/commit/0644691d02eb8dc0b48cd2132d0f6bd9d6a41f02))
* **ps:** harden connection lifecycle diagnostics ([b3a64d8](https://github.com/manuelgzz/LunarNX/commit/b3a64d8e227787c5829e79ff654061bdb0c25d1c))
* **ps:** reuse successful remote routes ([ddba4b1](https://github.com/manuelgzz/LunarNX/commit/ddba4b1b2f9d05f996dda794625da412244a44d1))
* **ps:** show touchpad gesture feedback ([83f1968](https://github.com/manuelgzz/LunarNX/commit/83f1968467452d2b806d11fbcaf381e5875549a0))
* **ps:** simplify local pairing account input ([79f0ff5](https://github.com/manuelgzz/LunarNX/commit/79f0ff5d2b0368f9309e198f1f685ac185e91579))
* **ps:** support offline account ID formats for local pairing ([ee0ae56](https://github.com/manuelgzz/LunarNX/commit/ee0ae56bc1752c1bbc601db965de1eb468c1d761))
* **ps:** support phone-assisted local pairing ([84bcc02](https://github.com/manuelgzz/LunarNX/commit/84bcc021cfb9a861d06b248b65399ad341db6afe))
* **ps:** unify local and remote connection planning ([e934dc4](https://github.com/manuelgzz/LunarNX/commit/e934dc4bfeba3ee6daf8b04602fc835287f10e1b))
* **stream:** show disconnect progress and timings ([518cd80](https://github.com/manuelgzz/LunarNX/commit/518cd80d173a18d0bb640471f6989115cb778e8d))
* **switch:** harden streaming and refresh UI ([d97b6d9](https://github.com/manuelgzz/LunarNX/commit/d97b6d900fbc7a4b22db3b16d40b5e24f86f3ba8))
* **ui:** add platform-scoped button mapping ([17474e7](https://github.com/manuelgzz/LunarNX/commit/17474e791d6395231a668719d46e8c4dbbeccb64))
* **ui:** add Xbox and PlayStation platform marks ([5573290](https://github.com/manuelgzz/LunarNX/commit/55732905da0be0a83f36d01e202d3872e42a3618))
* **ui:** align Borealis screens with Stitch design ([07bbb29](https://github.com/manuelgzz/LunarNX/commit/07bbb2912854abb8c0f9955e9b4f5c2e15337a4d))
* **ui:** block limited applet-mode launches ([dc4dd22](https://github.com/manuelgzz/LunarNX/commit/dc4dd22ee67dd7e816a551673a8b0e4ebfaf5ae4))
* **ui:** redesign about page ([a6dc1b1](https://github.com/manuelgzz/LunarNX/commit/a6dc1b1fd81bfa1d6db61d627025aaa001cea9e4))
* **ui:** refresh About and support pages ([e28fa07](https://github.com/manuelgzz/LunarNX/commit/e28fa07d8fe5d5e553af05befc081b52f698ab9b))
* **ui:** refresh About and support pages ([84d720b](https://github.com/manuelgzz/LunarNX/commit/84d720b3cd1377944029a9957de25398c38dc889))
* **updater:** show download speed and eta ([b044ba2](https://github.com/manuelgzz/LunarNX/commit/b044ba29245569da8839dbee507c986ee160ab92))
* **updater:** show gzip download progress dialog ([c40266a](https://github.com/manuelgzz/LunarNX/commit/c40266ad3ae858ebd16affbbbb220572f0439701))
* **xbox:** adapt stream policy to network path ([d54e0bd](https://github.com/manuelgzz/LunarNX/commit/d54e0bdd3b5f57fe12d555a0de3b0ee381e64eaf))
* **xbox:** reuse successful ICE paths ([a600fe7](https://github.com/manuelgzz/LunarNX/commit/a600fe75474abf85b0f84b1df183df2406d449a1))
* **xbox:** speed up stream exit and reuse ICE paths ([5a6eb0d](https://github.com/manuelgzz/LunarNX/commit/5a6eb0dd2ad57f2869b8a93fb4fb0095ecd4c9e6))
* **xcloud:** redesign cloud library browsing ([d8cb64a](https://github.com/manuelgzz/LunarNX/commit/d8cb64a4e1b13be6b70e11e97de097377010af9e))


### Bug Fixes

* **audio:** preserve Audren voice across source flush ([ff72a10](https://github.com/manuelgzz/LunarNX/commit/ff72a105094c8cdc621299b4a1fa1a7534c2ebef))
* avoid GPU lock inversion when stopping streams ([76a8a2f](https://github.com/manuelgzz/LunarNX/commit/76a8a2f5df0d26ecf4a7a357c60dd978bcec2e52))
* **chiaki:** preserve diagnostic patch recount ([2168cd3](https://github.com/manuelgzz/LunarNX/commit/2168cd3572a63c7623067d45f92a1d53ec8ba2d2))
* **ci:** make legacy libpeer patch apply after base patches ([59d04a6](https://github.com/manuelgzz/LunarNX/commit/59d04a622e17631610e33360e597274fe77d0c5e))
* **ci:** preserve nested libpeer dependency patches ([29d6d7c](https://github.com/manuelgzz/LunarNX/commit/29d6d7c722b340a450ab02388a61dbac242445b3))
* **ci:** trust container checkout workspace ([7af9fbf](https://github.com/manuelgzz/LunarNX/commit/7af9fbf8f5a53731bf0707cd0a1becee82b33fc6))
* **ci:** validate dynamic release versions ([4a92cd2](https://github.com/manuelgzz/LunarNX/commit/4a92cd290faf1bb945e71fdf9bba109489f3e041))
* **dev-build:** restore Switch downloads and stream exit ([49e44d9](https://github.com/manuelgzz/LunarNX/commit/49e44d9e9d8d22ecbb2648df09e5c05861b552ca))
* **dev-update:** compress builds and tolerate slow downloads ([c1d10b1](https://github.com/manuelgzz/LunarNX/commit/c1d10b1d06d07ce4d4889174703cb860a5eb6aa1))
* harden PS pairing and stream reliability ([8e208c5](https://github.com/manuelgzz/LunarNX/commit/8e208c5a1558605cd3946f1664f489fac59e2239))
* improve Xbox stream resilience and latency ([03d00e0](https://github.com/manuelgzz/LunarNX/commit/03d00e0c1ac601e352f74cdbc068f4bf4b8e1657))
* **libpeer:** use reproducible libsrtp headers ([79ffd81](https://github.com/manuelgzz/LunarNX/commit/79ffd81764d7b33af785da42fe354114628b3f64))
* **merge:** remove stale decoder policy reference ([48e8dc5](https://github.com/manuelgzz/LunarNX/commit/48e8dc56aa6c67531807e46dfde4661f0310401a))
* **merge:** restore shared decoder timestamp bookkeeping ([3a08b3a](https://github.com/manuelgzz/LunarNX/commit/3a08b3abf86ee9a47ceab22f94aedc1cf3924743))
* prevent deadlock when exiting a stream ([011a0c9](https://github.com/manuelgzz/LunarNX/commit/011a0c9a407d97798445d0268d3d712a72255960))
* **ps:** align local registration with Chiaki ([ca5f538](https://github.com/manuelgzz/LunarNX/commit/ca5f538f1d3026e15a5bab6c799f789269385bfc))
* **ps:** align persisted LAN pairing with Akira behavior ([4bf994b](https://github.com/manuelgzz/LunarNX/commit/4bf994b8beb404218ae9f7b41080052488df221e))
* **ps:** align PS4 loss recovery with Chiaki ([44f44e0](https://github.com/manuelgzz/LunarNX/commit/44f44e09527122c43f68b358ab350b83259a3d29))
* **ps:** align PS4 pairing firmware targets ([667da86](https://github.com/manuelgzz/LunarNX/commit/667da862829e064ca8bfc7c379cd31aa0bcecce9))
* **ps:** allow persisted LAN routes and periodic host probes ([58f878e](https://github.com/manuelgzz/LunarNX/commit/58f878ee38abbbcdbb3e4c6d6f5336fe8d4d4a70))
* **ps:** buffer video samples before media startup ([e6f378d](https://github.com/manuelgzz/LunarNX/commit/e6f378d73b994bfc6617734bc717701de20e871b))
* **ps:** close review gaps in pairing lifecycle and UX ([9e44d6f](https://github.com/manuelgzz/LunarNX/commit/9e44d6fbdd74c8cad843af0468abc56664a62bd8))
* **ps:** compile transport diagnostics out of release ([1823915](https://github.com/manuelgzz/LunarNX/commit/1823915c2229700e3c49ff545307df264fd470f6))
* **ps:** correct local pairing target and account check ([9e41dd3](https://github.com/manuelgzz/LunarNX/commit/9e41dd38c8f14412b4f56cbbce5b12116c3ba4a6))
* **ps:** defer recovery reset until keyframe ([5be9951](https://github.com/manuelgzz/LunarNX/commit/5be995178aaf2450808c42e18e9d81db5faa05d4))
* **ps:** forward remote play haptics to Switch rumble ([81fbcdf](https://github.com/manuelgzz/LunarNX/commit/81fbcdfa715395d8bdc57ee76e0044a88e6be76f))
* **ps:** handle packet stats wrap in long streams ([412d3e0](https://github.com/manuelgzz/LunarNX/commit/412d3e0fead4d9b743540497a0c7dee430c2da04))
* **ps:** harden first-frame media startup ([cb68128](https://github.com/manuelgzz/LunarNX/commit/cb681282036bf5e48488058a8d9144a88cc86c28))
* **ps:** harden local pairing and startup lifecycle ([35f2c4e](https://github.com/manuelgzz/LunarNX/commit/35f2c4e8272e7e348df742e6b7102fde5e6a8c5a))
* **ps:** harden local pairing recovery ([13b83fb](https://github.com/manuelgzz/LunarNX/commit/13b83fbd8e03645edfcdd151c3eef473a3a8feea))
* **ps:** harden post-pair streaming flow ([f7cd293](https://github.com/manuelgzz/LunarNX/commit/f7cd293e3708de5411dc5db17b8a1f62d34431a2))
* **ps:** harden PSN auth and phone sign-in ([11bddb0](https://github.com/manuelgzz/LunarNX/commit/11bddb0d592a9b5bc593689b9f2abac2d96b93e8))
* **ps:** increase applet socket capacity for pairing ([f63d4b5](https://github.com/manuelgzz/LunarNX/commit/f63d4b5ac49294d37208320a5307e8ff89f54fa0))
* **ps:** make present diagnostics reflect new frames ([27dbf85](https://github.com/manuelgzz/LunarNX/commit/27dbf85248877f77337eecc1475ab455b451a050))
* **ps:** preserve raw pairing failure logs ([3b333c1](https://github.com/manuelgzz/LunarNX/commit/3b333c1f040a16afbb340786101188b60506157f))
* **ps:** prevent background page from stealing stream focus ([1469ed5](https://github.com/manuelgzz/LunarNX/commit/1469ed5587f979a969e43cb1e27d4c5cd5e57a9c))
* **ps:** require accepted NVDEC packets ([73f0290](https://github.com/manuelgzz/LunarNX/commit/73f0290722a0ee499fb19e95404b9e9b37ada7e8))
* **ps:** require Base64 account ID for pairing ([0895702](https://github.com/manuelgzz/LunarNX/commit/0895702c6e06bd206f3f97648e00db98f3cb7f74))
* **ps:** reset decoder after recovery IDR failure ([5db056f](https://github.com/manuelgzz/LunarNX/commit/5db056fe3bed8a33fd0aab8db83cad43f5f03111))
* **ps:** restore remote connection reliability ([5673b42](https://github.com/manuelgzz/LunarNX/commit/5673b4290be0d3acd5b06415d852caa785ec0690))
* **ps:** restore remote reliability and reuse successful routes ([845e8d5](https://github.com/manuelgzz/LunarNX/commit/845e8d51572c26b47ae465ab30b4280e0ce343cc))
* **ps:** restore vibration for PS Remote Play ([42734d9](https://github.com/manuelgzz/LunarNX/commit/42734d9b9ba9bf43ea3aad38d940166d72585c18))
* **ps:** select console type for manual pairing ([1c96cd8](https://github.com/manuelgzz/LunarNX/commit/1c96cd8ce6bdb7860f448f76810ba4fa0bb9c30e))
* **ps:** separate local pairing identity from PSN ([0145454](https://github.com/manuelgzz/LunarNX/commit/0145454edd6ed64e9e098fd7b028fb4d73cc44df))
* **ps:** simplify pairing account wording ([deecab9](https://github.com/manuelgzz/LunarNX/commit/deecab9f06b927c9b1e98cfdc2a38fe787c3d513))
* **ps:** stabilize pairing focus navigation ([c8d4fc9](https://github.com/manuelgzz/LunarNX/commit/c8d4fc90a00429b7d8e4eaf627452a066f2c604e))
* **ps:** support offline-activated console pairing ([fe9fa15](https://github.com/manuelgzz/LunarNX/commit/fe9fa15dedec448718e33528a4f4612910967b3c))
* **release:** derive dev version from app version ([548be62](https://github.com/manuelgzz/LunarNX/commit/548be624672e826b7d19a08dd8a80f63a05cb05b))
* **release:** make versions and packages future-proof ([9ccaef7](https://github.com/manuelgzz/LunarNX/commit/9ccaef75321e7d4cc79cc01fb41aeccc7581c692))
* **release:** make versions and packages future-proof ([6431b64](https://github.com/manuelgzz/LunarNX/commit/6431b6487814ed4493841950aa907843cfba3a57))
* **release:** match Sphaira package layout ([dff50b9](https://github.com/manuelgzz/LunarNX/commit/dff50b98da6028ba90d964f727e7ce8864fb2d97))
* **release:** match Sphaira package layout ([5f5e5ec](https://github.com/manuelgzz/LunarNX/commit/5f5e5ec60d0c5e0f54c4ccfdfcb51dbaa23cc875))
* **release:** reject builds without upload token proof ([ff98159](https://github.com/manuelgzz/LunarNX/commit/ff98159661e0f019cd6ae59fa77bb0aba8f740a4))
* **release:** verify upload token in linked artifact ([f221933](https://github.com/manuelgzz/LunarNX/commit/f221933188ff00fe29e6a02344aaff9ca747ec14))
* **render:** keep NV12 padding out of texture descriptors ([ff9f077](https://github.com/manuelgzz/LunarNX/commit/ff9f077a40ff9a0f0dc84af917db9a784a810569))
* **settings:** separate Xbox and PlayStation controls ([b66073a](https://github.com/manuelgzz/LunarNX/commit/b66073a754b3648fd77825186ecdad0df3158c79))
* **stream:** create stop spinner only on exit ([d543ade](https://github.com/manuelgzz/LunarNX/commit/d543ade0ba81e5b686f8d50719f65d159e5c9a07))
* **stream:** fix 720p green line and settings crash ([f07f1d7](https://github.com/manuelgzz/LunarNX/commit/f07f1d78b9fec359cef1686ebdd1c75b21130ca4))
* **stream:** give quick menu exclusive input ownership ([a75e81e](https://github.com/manuelgzz/LunarNX/commit/a75e81e9fffd8c8e6f943219c0b6324cf34c29b9))
* **stream:** improve loss recovery diagnostics ([f88e5f5](https://github.com/manuelgzz/LunarNX/commit/f88e5f5d0391753471fd8de3353a4ac4fa1102b8))
* **streaming:** align PS/Xbox recovery with tested build ([65ce465](https://github.com/manuelgzz/LunarNX/commit/65ce4659a2383022fddd71e348dc4e80e16b1fbf))
* **stream:** recover sessions after foreground resume ([a3fca55](https://github.com/manuelgzz/LunarNX/commit/a3fca55aaf734940c166dc672a0f234e90a1e298))
* **stream:** suppress complete Xbox input frames ([9f60160](https://github.com/manuelgzz/LunarNX/commit/9f6016030f5c99daccdfd424b586dcfc382c9e25))
* **stream:** suspend presentation watchdog for child UI ([eb50322](https://github.com/manuelgzz/LunarNX/commit/eb50322a929d7d1cca79e01f9bf8fe01f98bd730))
* **stream:** wait for teardown before leaving stream view ([59fefcc](https://github.com/manuelgzz/LunarNX/commit/59fefcc3444b3871a3b09bf8c98c9630d15de042))
* **tests:** select available C++ compiler in CI ([362ccb2](https://github.com/manuelgzz/LunarNX/commit/362ccb2102e959753714827cefea7e4787e436e8))
* **ui:** align about metadata and expand credits ([c43cf90](https://github.com/manuelgzz/LunarNX/commit/c43cf903939b90fba3bd046c911c132c35e77b9a))
* **ui:** close interaction review gaps ([b61ca15](https://github.com/manuelgzz/LunarNX/commit/b61ca155d30748879daecd6fbe3476a5efae9fcf))
* **ui:** coordinate async teardown with view lifecycle ([e3268a1](https://github.com/manuelgzz/LunarNX/commit/e3268a13408c26579d841132c80b71d3aee76685))
* **ui:** coordinate async teardown with view lifecycle ([e203d1a](https://github.com/manuelgzz/LunarNX/commit/e203d1a857fc2d949392463905edd859f601ac30))
* **ui:** correct about page focus and spacing ([c2b1dae](https://github.com/manuelgzz/LunarNX/commit/c2b1dae6cb28708775c6d1e757e6dc87fa4bed34))
* **ui:** harden stream exit and connection cancellation ([52d203f](https://github.com/manuelgzz/LunarNX/commit/52d203fe971b9390669b46386b2822e5fce3f71e))
* **ui:** harden stream exit rendering ([9f7a012](https://github.com/manuelgzz/LunarNX/commit/9f7a01280db1fca5a5069da487e45c022bbf071b))
* **ui:** keep PS pairing keypad actions visible ([c1978d3](https://github.com/manuelgzz/LunarNX/commit/c1978d32276427b4394f1056bde95455805339ef))
* **ui:** preserve Borealis dialog animations ([7223402](https://github.com/manuelgzz/LunarNX/commit/7223402ba93a3d9104b3e141a5d2767542559a7c))
* **ui:** preserve readable generated release notes ([aa86dee](https://github.com/manuelgzz/LunarNX/commit/aa86dee4e6240b14c0f6b7fc229ed028a5ab00fb))
* **ui:** prevent footer and sidebar text overflow ([6467a14](https://github.com/manuelgzz/LunarNX/commit/6467a14dba0df5b4b8379d036e42f149d56d5280))
* **ui:** remove duplicate page titles ([a227267](https://github.com/manuelgzz/LunarNX/commit/a227267af004121011256019d45a7cd793e951ef))
* **ui:** serialize stream connection cancellation ([69dfd2e](https://github.com/manuelgzz/LunarNX/commit/69dfd2ec039d87d82eb6d17eefed1d4d51bfa7b5))
* **ui:** show 0.3.0 release notes in About ([bac78be](https://github.com/manuelgzz/LunarNX/commit/bac78be29f90f8507c3a9539a1c3012cb1ea898e))
* **ui:** simplify right-side content hierarchy ([3257eaf](https://github.com/manuelgzz/LunarNX/commit/3257eaffd26c48476000e1fd51c2cfb67760e2fb))
* **ui:** suppress stream input under child pages ([cee3faf](https://github.com/manuelgzz/LunarNX/commit/cee3faf8cca064e6bc81e3bda44feb1ef0f12699))
* **xbox:** adapt RTP recovery to network quality ([c6da4c1](https://github.com/manuelgzz/LunarNX/commit/c6da4c16c9bf15469aef9f53b9afcfd8f169f4b2))
* **xbox:** bound input and video latency recovery ([19567af](https://github.com/manuelgzz/LunarNX/commit/19567af30f58ac26b39635bd2e7d3265ef64f160))
* **xbox:** cancel network waits during stream exit ([7ab4ef1](https://github.com/manuelgzz/LunarNX/commit/7ab4ef1b40ecb2861c038dc96257e91fb9407434))
* **xbox:** close input and watchdog recovery gaps ([ad7de3e](https://github.com/manuelgzz/LunarNX/commit/ad7de3eb2217846ca365b5edeb8e10265446c3a3))
* **xbox:** drop SRTP failure threshold from recovery trigger ([e948ef8](https://github.com/manuelgzz/LunarNX/commit/e948ef8b19769fc251f3743ba456bef6e7bec0b5))
* **xbox:** give home RTP gaps a bounded recovery window ([9cc9e50](https://github.com/manuelgzz/LunarNX/commit/9cc9e503187fe4816dd450e1e1defccafb16a724))
* **xbox:** make gamepad input realtime ([1724eee](https://github.com/manuelgzz/LunarNX/commit/1724eeebd1179ed37646bba0a40f09f0b03f213c))
* **xbox:** make input transitions and SCTP recovery reliable ([c0b43b8](https://github.com/manuelgzz/LunarNX/commit/c0b43b8d321c85e2ca867de160cf24a2d819c703))
* **xbox:** preserve forced input snapshot generation ([ffc3af2](https://github.com/manuelgzz/LunarNX/commit/ffc3af20ca554ed55087e0ddbc30885044f4b101))
* **xbox:** preserve input delivery and sequence continuity ([c332ffa](https://github.com/manuelgzz/LunarNX/commit/c332ffacedd8144dd06d42887f9d1dc0f6520299))
* **xbox:** preserve input transitions under backpressure ([235b716](https://github.com/manuelgzz/LunarNX/commit/235b7169886613904ba275ce628922c4fb5acc1a))
* **xbox:** preserve media on token refresh failure ([005dc18](https://github.com/manuelgzz/LunarNX/commit/005dc18392ee0677332e245fdf43835428facef7))
* **xbox:** preserve recovery budget on high RTT paths ([8b5d363](https://github.com/manuelgzz/LunarNX/commit/8b5d3630b555306a892f98de48811b36299f402d))
* **xbox:** protect cloud RTP recovery from burst loss ([4de7b97](https://github.com/manuelgzz/LunarNX/commit/4de7b978e1e9e7fc9f69367ebd1430f9777f11ef))
* **xbox:** recover long-running video stream stalls ([62ee5b2](https://github.com/manuelgzz/LunarNX/commit/62ee5b237832016a1705c01ca4845f1578ee516a))
* **xbox:** recover stream stalls and preserve input delivery ([76054fa](https://github.com/manuelgzz/LunarNX/commit/76054faa3acd46194a2fbc8122a328e1dc02e177))
* **xbox:** refresh streaming token after keepalive auth failure ([e9a6152](https://github.com/manuelgzz/LunarNX/commit/e9a6152768820cc5a6281b677d9e7d5073bfe2d1))
* **xbox:** restore cloud retransmission jitter budget ([7746b69](https://github.com/manuelgzz/LunarNX/commit/7746b6966900e167695f687fe7c38044630930e6))
* **xbox:** restore Home WAN recovery and latest-frame presentation ([1a9b361](https://github.com/manuelgzz/LunarNX/commit/1a9b361409a77f91e0fd01111999bb5aaa1b85b3))
* **xbox:** restore Home WAN recovery and latest-frame presentation ([6bd2583](https://github.com/manuelgzz/LunarNX/commit/6bd2583f62aafe6c22e0cfc124ce3bc22ff1a609))
* **xbox:** restore known-good UDP receive buffer ([df38e1f](https://github.com/manuelgzz/LunarNX/commit/df38e1f3ddb97de72e8d0d08d9ab0bbd5cdc9e3a))
* **xbox:** restore stable input cadence ([275a170](https://github.com/manuelgzz/LunarNX/commit/275a1701c4a8dec8d06a2ce00b078a887da5ca09))
* **xbox:** retry latest input and reset audio source ([2782065](https://github.com/manuelgzz/LunarNX/commit/278206590696b521b5e8ae534138483229082096))
* **xbox:** stabilize low-latency audio playback ([b58232b](https://github.com/manuelgzz/LunarNX/commit/b58232bee8b3106269694caec960422d0867aa33))
* **xbox:** stabilize realtime streaming and recovery ([04b48b1](https://github.com/manuelgzz/LunarNX/commit/04b48b1f509096868c230e07bfddda9a41beaef1))
* **xbox:** stabilize realtime streaming and recovery ([c8e4fb9](https://github.com/manuelgzz/LunarNX/commit/c8e4fb977b8eac9877ddebad3fe60cb7376bd3d2))


### Performance Improvements

* **ps:** buffer decoded frames across present jitter ([6ae4b59](https://github.com/manuelgzz/LunarNX/commit/6ae4b59a7c15178161ab2bf8b7eb8bad039bdb73))
* **ps:** stabilize 30 Mbps high-bitrate streaming ([43bd12c](https://github.com/manuelgzz/LunarNX/commit/43bd12ca3fa1717177ba892b076ef9405f7c3802))
* **stream:** separate Xbox and PlayStation video paths ([6f55c77](https://github.com/manuelgzz/LunarNX/commit/6f55c77d2ebf7f551caa33a9cd382d705eeccf12))


### Documentation

* add Chinese getting-started guide ([9158bc3](https://github.com/manuelgzz/LunarNX/commit/9158bc37055710420a667ee0e1b3cbbc86a35c35))
* add concise QQ group usage notice ([b5ac46c](https://github.com/manuelgzz/LunarNX/commit/b5ac46cbafca77e7fe8d388018ea69ce1adbaaa6))
* add Switch hardware screenshots ([5d9aba2](https://github.com/manuelgzz/LunarNX/commit/5d9aba2b2bb4d5f6539f1e3d46cc8fde1ef6d221))
* clarify stable ZIP extraction path ([5b60e36](https://github.com/manuelgzz/LunarNX/commit/5b60e363bf4f1081e8a95d9e133d702bbcf721c3))
* clarify Xbox Guide controls ([1308cc9](https://github.com/manuelgzz/LunarNX/commit/1308cc9dd81d1c51c349e5c3e457703c54141689))
* link active port and preserve cross-device development handoff ([3b6c322](https://github.com/manuelgzz/LunarNX/commit/3b6c322f43cfe5c1c8b581838f6be032552951e0))
* link published decoder controls PR and validation ([18100c6](https://github.com/manuelgzz/LunarNX/commit/18100c6e595559bd7ef21a766c28d2b8c5ddcfbd))
* note legacy release asset names ([5d59d36](https://github.com/manuelgzz/LunarNX/commit/5d59d36596adbb28f005d15a0f2cfd6042c70304))
* point to develop and finalize consolidation after platform validation ([92f0a6e](https://github.com/manuelgzz/LunarNX/commit/92f0a6e9eb64824faed52f9fa18a46fda0861a00))
* preserve development context and require continuity updates ([020c778](https://github.com/manuelgzz/LunarNX/commit/020c77829c4550dad9e49a2094f10ee187dcec66))
* record CI results for stable Android signing ([4dbaf35](https://github.com/manuelgzz/LunarNX/commit/4dbaf350614bf9a264f7402f25be92fe83c3f06c))
* record successful consolidation and retired branches ([b6ac03d](https://github.com/manuelgzz/LunarNX/commit/b6ac03d37365cba5310f76275dbc981bef229377))
* record successful consolidation and retired branches ([c1cd30f](https://github.com/manuelgzz/LunarNX/commit/c1cd30ff475392f356a00ef44744f61a9d313c40))
* refocus README on users ([78f451c](https://github.com/manuelgzz/LunarNX/commit/78f451cf5a751c266b13d8c71d81cde665812172))
* refresh project status and references ([2bc7b16](https://github.com/manuelgzz/LunarNX/commit/2bc7b166a3ef772feb65b75b68d2dda010cb267e))
* refresh project status and references ([3abda3a](https://github.com/manuelgzz/LunarNX/commit/3abda3a35305aaeb271bcc9c859080d623b79dc4))
* refresh readme and add app store banner ([714f3aa](https://github.com/manuelgzz/LunarNX/commit/714f3aaaf927ceb37078bf91a713f3abad92a8bb))
* refresh README and add app store banner ([9e5d71c](https://github.com/manuelgzz/LunarNX/commit/9e5d71c04a77247745035b8c9e889b3fddba582d))
* refresh release README ([4cddf27](https://github.com/manuelgzz/LunarNX/commit/4cddf274cdc82e5508503e1ae03b7ce162b76b31))
* **release:** document stable Sphaira install paths ([e669feb](https://github.com/manuelgzz/LunarNX/commit/e669feb0a02c52cc90e897084219b9a3513ea41c))
* **release:** refresh 0.3.0 changelog ([ad94bd8](https://github.com/manuelgzz/LunarNX/commit/ad94bd8e56f2a60076ebc71e31afbe1eea2e29bb))
* **release:** summarize 0.4.0 changelog ([e31480f](https://github.com/manuelgzz/LunarNX/commit/e31480f3f4ac3c222002ef7fadf4b5c48fdfdc8d))
* **stream:** PS/Xbox video decoder separation design ([c9db685](https://github.com/manuelgzz/LunarNX/commit/c9db68563853bffb000f779fb851e67a1e4128b2))
* **ui:** credit XStreaming and PeaSyo ([f61e7c7](https://github.com/manuelgzz/LunarNX/commit/f61e7c7ed287f97454f1d69b5b0ef149c78dec68))


### Code Refactoring

* **stream:** own PS/Xbox decode policy in separate decoder classes ([a26a02c](https://github.com/manuelgzz/LunarNX/commit/a26a02ce8109075a38b14f16bc6b88068bdbbf05))
* **ui:** add Borealis workspace shell helpers ([1c52295](https://github.com/manuelgzz/LunarNX/commit/1c52295dcf7104b81acb07cc865698b52b0ad21b))
* **ui:** align all activities with approved visual system ([6909843](https://github.com/manuelgzz/LunarNX/commit/6909843eb97707685ee83651bd2f8f67fc6e9813))
* **ui:** complete platform workspace redesign ([34a93a1](https://github.com/manuelgzz/LunarNX/commit/34a93a1f07f574d7a17829c10ba8ff682b7322e7))
* **ui:** rebuild platform workspace launcher ([0a7251e](https://github.com/manuelgzz/LunarNX/commit/0a7251e54a54f9e045cb375e24161fb7eba575c2))
* **ui:** rebuild Xbox workspace navigation ([3c99570](https://github.com/manuelgzz/LunarNX/commit/3c995702fa790cf94ba9401a4a8f1325ce88cf63))
* **ui:** restore classic layout ([f52db24](https://github.com/manuelgzz/LunarNX/commit/f52db24076dbf9a2939f0b7d93294893b2d9756e))

## [0.3.3](https://github.com/thinkzhou/LunarNX/compare/v0.3.2...v0.3.3) (2026-09-07)


### Bug Fixes

* **xbox:** restore Home WAN recovery and latest-frame presentation ([1a9b361](https://github.com/thinkzhou/LunarNX/commit/1a9b361409a77f91e0fd01111999bb5aaa1b85b3))
* **xbox:** restore Home WAN recovery and latest-frame presentation ([6bd2583](https://github.com/thinkzhou/LunarNX/commit/6bd2583f62aafe6c22e0cfc124ce3bc22ff1a609))

## [0.3.2](https://github.com/thinkzhou/LunarNX/compare/v0.3.1...v0.3.2) (2026-09-07)


### Bug Fixes

* avoid GPU lock inversion when stopping streams ([76a8a2f](https://github.com/thinkzhou/LunarNX/commit/76a8a2f5df0d26ecf4a7a357c60dd978bcec2e52))
* prevent deadlock when exiting a stream ([011a0c9](https://github.com/thinkzhou/LunarNX/commit/011a0c9a407d97798445d0268d3d712a72255960))
* **release:** match Sphaira package layout ([dff50b9](https://github.com/thinkzhou/LunarNX/commit/dff50b98da6028ba90d964f727e7ce8864fb2d97))
* **release:** match Sphaira package layout ([5f5e5ec](https://github.com/thinkzhou/LunarNX/commit/5f5e5ec60d0c5e0f54c4ccfdfcb51dbaa23cc875))

## [0.3.1](https://github.com/thinkzhou/LunarNX/compare/v0.3.0...v0.3.1) (2026-09-04)

LunarNX 0.3.1 focuses on faster reconnection, safer stream teardown, and
predictable release packaging. It reuses successful Xbox and PlayStation
network routes, improves cancellation and UI lifecycle handling, and publishes
stable assets for Sphaira installation.

### Features

* **ps:** reuse successful remote routes ([ddba4b1](https://github.com/thinkzhou/LunarNX/commit/ddba4b1b2f9d05f996dda794625da412244a44d1))
* **xbox:** reuse successful ICE paths ([a600fe7](https://github.com/thinkzhou/LunarNX/commit/a600fe75474abf85b0f84b1df183df2406d449a1))
* **xbox:** speed up stream exit and reuse ICE paths ([5a6eb0d](https://github.com/thinkzhou/LunarNX/commit/5a6eb0dd2ad57f2869b8a93fb4fb0095ecd4c9e6))


### Bug Fixes

* **ps:** restore remote connection reliability ([5673b42](https://github.com/thinkzhou/LunarNX/commit/5673b4290be0d3acd5b06415d852caa785ec0690))
* **ps:** restore remote reliability and reuse successful routes ([845e8d5](https://github.com/thinkzhou/LunarNX/commit/845e8d51572c26b47ae465ab30b4280e0ce343cc))
* **release:** make versions and packages future-proof ([9ccaef7](https://github.com/thinkzhou/LunarNX/commit/9ccaef75321e7d4cc79cc01fb41aeccc7581c692))
* **ui:** coordinate async teardown with view lifecycle ([e3268a1](https://github.com/thinkzhou/LunarNX/commit/e3268a13408c26579d841132c80b71d3aee76685))
* **ui:** harden stream exit and connection cancellation ([52d203f](https://github.com/thinkzhou/LunarNX/commit/52d203fe971b9390669b46386b2822e5fce3f71e))
* **ui:** harden stream exit rendering ([9f7a012](https://github.com/thinkzhou/LunarNX/commit/9f7a01280db1fca5a5069da487e45c022bbf071b))
* **ui:** preserve Borealis dialog animations ([7223402](https://github.com/thinkzhou/LunarNX/commit/7223402ba93a3d9104b3e141a5d2767542559a7c))
* **ui:** preserve readable generated release notes ([aa86dee](https://github.com/thinkzhou/LunarNX/commit/aa86dee4e6240b14c0f6b7fc229ed028a5ab00fb))
* **ui:** serialize stream connection cancellation ([69dfd2e](https://github.com/thinkzhou/LunarNX/commit/69dfd2ec039d87d82eb6d17eefed1d4d51bfa7b5))
* **ui:** show 0.3.0 release notes in About ([bac78be](https://github.com/thinkzhou/LunarNX/commit/bac78be29f90f8507c3a9539a1c3012cb1ea898e))
* **xbox:** cancel network waits during stream exit ([7ab4ef1](https://github.com/thinkzhou/LunarNX/commit/7ab4ef1b40ecb2861c038dc96257e91fb9407434))


### Documentation

* add Chinese getting-started guide ([9158bc3](https://github.com/thinkzhou/LunarNX/commit/9158bc37055710420a667ee0e1b3cbbc86a35c35))
* add concise QQ group usage notice ([b5ac46c](https://github.com/thinkzhou/LunarNX/commit/b5ac46cbafca77e7fe8d388018ea69ce1adbaaa6))
* clarify stable ZIP extraction path ([5b60e36](https://github.com/thinkzhou/LunarNX/commit/5b60e363bf4f1081e8a95d9e133d702bbcf721c3))
* note legacy release asset names ([5d59d36](https://github.com/thinkzhou/LunarNX/commit/5d59d36596adbb28f005d15a0f2cfd6042c70304))
* refresh project status and references ([2bc7b16](https://github.com/thinkzhou/LunarNX/commit/2bc7b166a3ef772feb65b75b68d2dda010cb267e))
* refresh project status and references ([3abda3a](https://github.com/thinkzhou/LunarNX/commit/3abda3a35305aaeb271bcc9c859080d623b79dc4))
* **release:** document stable Sphaira install paths ([e669feb](https://github.com/thinkzhou/LunarNX/commit/e669feb0a02c52cc90e897084219b9a3513ea41c))
* **ui:** credit XStreaming and PeaSyo ([f61e7c7](https://github.com/thinkzhou/LunarNX/commit/f61e7c7ed287f97454f1d69b5b0ef149c78dec68))

## [0.3.0](https://github.com/thinkzhou/LunarNX/compare/v0.2.0...v0.3.0) (2026-09-02)

LunarNX 0.3.0 is a major streaming-stability update for Xbox and PlayStation. It improves controller responsiveness, audio/video recovery, PlayStation pairing and startup, rendering correctness, and the overall in-stream experience.

### Xbox streaming

- Added network-aware streaming policies for low-latency local play and more variable cloud connections.
- Improved recovery from burst packet loss, high RTT, RTP gaps, token refresh failures, and long-running video stalls.
- Made controller delivery more reliable under data-channel backpressure, reducing sticky or missing button transitions.
- Stabilized low-latency audio playback and recovery after audio-source resets.

### PlayStation streaming

- Expanded local PS4 and PS5 discovery, pairing, wake-up, and connection handling, including offline-activated consoles and phone-assisted pairing.
- Improved saved-console routing and host probing so previously paired consoles can reconnect more reliably.
- Hardened PSN and local session startup, teardown, first-frame handling, decoder recovery, and packet-loss recovery.
- Improved high-bitrate playback, including 30 Mbps streams, while keeping PlayStation buffering and recovery independent from Xbox policy.
- Restored Remote Play vibration on Switch, including PS5 DualSense haptics and legacy rumble, with clean shutdown when a stream ends.

### Video, audio, and controls

- Fixed the green line at the bottom of 720p video by keeping NV12 storage padding out of visible texture dimensions.
- Fixed false renderer-stall recovery when opening in-stream settings or button mapping, preventing unintended disconnects and crashes while keeping transport and decoder recovery active.
- Separated Xbox and PlayStation video paths so each protocol can apply its own buffering and recovery policy.
- Improved audio/video queue handling, hardware-decoder acceptance checks, and keyframe recovery after corruption or loss.
- Added independent Xbox and PlayStation button-mapping profiles.
- Prevented the in-stream quick menu from leaking input to the remote console and made stream exit progress clearer.

### Interface and distribution

- Redesigned Xbox Cloud Gaming library browsing.
- Rebuilt the in-app About area with controller-friendly Project, Changelog, Community, and Support pages.
- Added direct project and release links, QQ and Discord community access, and optional support QR codes.
- Added clearer update download progress, speed, and ETA reporting.
- Added versioned GitHub releases with NRO and ZIP packages plus SHA-256 checksums.
- Added a Sphaira app-store banner and refreshed the English and Simplified Chinese installation and feature documentation.
- Added clearer diagnostics for connection, media startup, loss recovery, and shutdown failures.

### Notes

- LunarNX remains early-stage homebrew software. PlayStation local pairing and streaming need broader testing across console models, firmware versions, and network environments.
- Launch LunarNX through Homebrew Menu title override/full-memory mode; Applet Mode does not provide enough memory for reliable streaming and hardware decoding.

## 0.2.0 (2026-08-14)

This version is the baseline for automated releases.
