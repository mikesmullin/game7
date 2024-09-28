import { glob } from 'glob'
import cbFs from 'fs';
import fs from 'fs/promises';
import path from 'path';
import { spawn } from 'child_process';
import { fileURLToPath } from 'url';

const isWin = process.platform === "win32";
const isMac = process.platform === "darwin";
const isNix = process.platform === "linux";
const __dirname = path.dirname(fileURLToPath(import.meta.url));
const CONCURRENCY = 48; // threadripper ftw!
// NOTICE: assumes glslc is in path
const GLSLC_PATH = isWin ? path.join('glslc.exe') : path.join('glslc');
const BUILD_PATH = "build";
//const CPP_COMPILER_PATH = "clang++";
const C_COMPILER_PATH = 'clang';
const RX_EXT = /\.[\w\d]{1,3}$/i;
const RX_C = /\.c$/i;
const OUT_FILE = "compile_commands.json";
const abs = (...args) => path.join(...args);
const workspaceFolder = path.join(__dirname, '..');
const rel = (...args) =>
  path.relative(path.join(workspaceFolder, BUILD_PATH), path.join(...args));
const DEBUG_COMPILER_ARGS = [
  '-O0',
  // '-gdwarf', // adds gdb support
  // TODO: do we need to pass `-debug` to the linker? `-Xlinker -debug`?
];
const C_COMPILER_ARGS = [];
C_COMPILER_ARGS.push('-m64');
const CPP_COMPILER_ARGS = [];
CPP_COMPILER_ARGS.push('-m64');
const C_COMPILER_INCLUDES = [];
const CPP_COMPILER_INCLUDES = [];

const ENGINE_ONLY = [
  'src/main.c',
  'src/lib/Audio.c',
  'src/lib/Engine.c',
  'src/lib/File.c',
  'src/lib/Finger.c',
  'src/lib/Gamepad.c',
  'src/lib/HotReload.c',
  'src/lib/Keyboard.c',
  'src/lib/SDL.c',
  'src/lib/Shader.c',
  'src/lib/Time.c',
  'src/lib/Vulkan.c',
  'src/lib/VulkanWrapper.c',
  'src/lib/Window.c',
  'vendor/cmixer-076653c/include/cmixer.c',
];

if (isWin) {
  C_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'sdl-2.26.1', 'include')}`);
  C_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'cglm-0.9.2', 'include')}`);
  C_COMPILER_INCLUDES.push(`-I${abs('C:', 'VulkanSDK', '1.3.236.0', 'Include')}`);
  C_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'volk-1.3.270', 'include')}`);
}
else if (isNix) {
  // CPP_COMPILER_INCLUDES.push(`-I/${abs('usr', 'include', 'vulkan')}`);
  // CPP_COMPILER_INCLUDES.push(`-I/${abs('usr', 'include', 'lua5.4')}`);
}
else if (isMac) {
  // CPP_COMPILER_INCLUDES.push(`-I/${abs('opt', 'homebrew', 'Cellar', 'sdl2', '2.30.0', 'include')}`);
  // CPP_COMPILER_INCLUDES.push(`-I/${abs('opt', 'homebrew', 'Cellar', 'lua', '5.4.6', 'include', 'lua5.4')}`);
}
// CPP_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'glm-0.9.9.8')}`);
// CPP_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'tinyobjloader', 'include')}`);
C_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'stb-2.29', 'include')}`);
C_COMPILER_INCLUDES.push(`-I${rel(workspaceFolder, 'vendor', 'cmixer-076653c', 'include')}`);
const LINKER_LIBS = [];
if (isWin) {
  // LINKER_LIBS.push('-l', 'user32');
  // LINKER_LIBS.push('-l', 'shell32');
  // LINKER_LIBS.push('-l', 'gdi32');
}
else if (isNix) {
}
else if (isMac) {
}
// NOTICE: we can lookup the compiler flags using command `sdl2-config --cflags --libs`
const LINKER_LIB_PATHS = [];
if (isWin) {
  LINKER_LIB_PATHS.push('-L', `${rel(workspaceFolder, 'vendor', 'sdl-2.26.1', 'lib', 'x64')}`, '-l', 'SDL2');
}
else if (isNix) {
  // LINKER_LIB_PATHS.push('-D_REENTRANT', '-lSDL2');
  // LINKER_LIB_PATHS.push('-lvulkan');
  // LINKER_LIB_PATHS.push('-L', `${rel(workspaceFolder, 'vendor', 'protobuf-25.2', 'nix', 'lib')}`, '-l', 'protobuf-lite');
  // LINKER_LIB_PATHS.push('-llua5.4');
}
else if (isMac) {
  // LINKER_LIB_PATHS.push(`-L/${abs('opt', 'homebrew', 'Cellar', 'sdl2', '2.30.0', 'lib')}`, '-D_REENTRANT', '-lSDL2');
  // LINKER_LIB_PATHS.push('-lvulkan');
  // LINKER_LIB_PATHS.push('-L', `${rel(workspaceFolder, 'vendor', 'protobuf-25.2', 'mac', 'lib')}`, '-l', 'protobuf-lite');
  // LINKER_LIB_PATHS.push(`-L/${abs('opt', 'homebrew', 'Cellar', 'lua', '5.4.6', 'lib')}`, '-llua5.4');
}
const COMPILER_TRANSLATION_UNITS = [
  rel(workspaceFolder, 'src', 'components', '*.c'),
  rel(workspaceFolder, 'src', 'lib', '*.c'),
  rel(workspaceFolder, 'src', 'game', '*.c'),
  rel(workspaceFolder, 'src', 'game', 'menu', '*.c'),
  // rel(workspaceFolder, 'src', 'proto', '*.cc'),
  rel(workspaceFolder, 'vendor', 'cmixer-076653c', 'include', '*.c'),
];
// const COMPILER_TRANSLATION_UNITS_DLL = [
//   rel(workspaceFolder, 'src', 'lib', '*.c'),
//   rel(workspaceFolder, 'src', 'game', '*.c'),
// ];
const C_CONDITIONAL_COMPILER_ARGS = (src) => {
  if (src.includes('cmixer')) {
    return ['-Wno-deprecated-declarations'];
  }
  return [];
};

const nixPath = (p) =>
  path.posix.normalize(p.replace(/\\/g, '/'));

const generate_clangd_compile_commands = async () => {
  console.log('scanning directory...');
  const unit_files = await glob('{src,tests}/**/*.c');

  console.debug('unit_files: ', unit_files);

  const compile_commands = [];

  for (const unit_file of unit_files) {
    compile_commands.push({
      directory: path.join(workspaceFolder, BUILD_PATH),
      arguments: [
        C_COMPILER_PATH,
        //...CPP_COMPILER_ARGS,
        ...C_COMPILER_ARGS,
        ...C_COMPILER_INCLUDES,
        '-c',
        '-o', `${unit_file}.o`,
        rel(unit_file),
      ],
      file: rel(workspaceFolder, unit_file),
    });
  }

  console.log(`writing ${OUT_FILE}...`)
  await fs.writeFile(OUT_FILE, JSON.stringify(compile_commands, null, 2));

  console.log('done making.');
};

const child_spawn = async (cmd, args = [], opts = {}) => {
  const cwd = path.relative(process.cwd(), path.join(workspaceFolder, BUILD_PATH));
  // console.log(`cd ${cwd}`);
  // console.log(`${opts.stdin ? `type ${opts.stdin} | ` : ''}${cmd} ${args.join(' ')}${opts.stdout ? ` > ${opts.stdout}` : ''}`);
  let stdin, stdout;
  const stdio = ['inherit', 'inherit', 'inherit'];
  if (opts.stdin) {
    stdio[0] = 'pipe';
    stdin = cbFs.createReadStream(path.join(workspaceFolder, 'assets', 'proto', 'addressbook.pb'));
  }
  if (opts.stdout) {
    stdio[1] = 'pipe';
    stdout = await cbFs.createWriteStream(path.join(workspaceFolder, 'assets', 'proto', 'addressbook.bin'));
  }
  const child = spawn(cmd, args, { cwd, stdio });
  if (opts.stdin) {
    stdin.pipe(child.stdin);
  }
  if (opts.stdout) {
    child.stdout.pipe(stdout);
  }
  const code = await new Promise((ok) => {
    child.on('close', async (code) => {
      if (code !== 0) {
        console.log(`process exited with code ${code}`);
      }
      ok(code);
    });
  });
  return code;
};

const promiseBatch = async function* (concurrency, list, fn) {
  for (let p = [], i = 0, l = list.length; i < l || p.length > 0;) {
    if (i < l) {
      let _p;
      _p = fn(list[i]).then(r => [_p.__id, r]);
      _p.__id = i++;
      if (p.push(_p) < concurrency) {
        continue;
      }
    }
    const [id, res] = await Promise.race(p);
    p = p.filter(x => x.__id !== id);
    yield res;
  }
};

const all = async () => {
  await clean();
  await copy_dlls();
  await shaders();
  await protobuf();
  await compile('main');
  await compile_reload("src/game/Logic.c.dll");
  await run('main');
};

const copy_dlls = async () => {
  const srcs = [];
  if (isWin) {
    srcs.push(path.join(workspaceFolder, 'vendor', 'sdl-2.26.1', 'lib', 'x64', 'SDL2.dll'));
    // srcs.push(path.join(workspaceFolder, 'vendor', 'protobuf-25.2', 'win', 'x64', 'libprotobuf-lite.dll'));
    // srcs.push(path.join(workspaceFolder, 'vendor', 'lua-5.4.2', 'x64', 'lua54.dll'));
  }
  const dest = path.join(workspaceFolder, BUILD_PATH);
  for (const src of srcs) {
    await fs.copyFile(src, path.join(dest, path.basename(src)));
  }
};

const shaders = async () => {
  await child_spawn(GLSLC_PATH,
    ['../assets/shaders/simple_shader.vert', '-o', '../assets/shaders/simple_shader.vert.spv']);

  await child_spawn(GLSLC_PATH,
    ['../assets/shaders/simple_shader.frag', '-o', '../assets/shaders/simple_shader.frag.spv']);
};

const protobuf = async () => {
  // const PROTOC_PATH =
  //   isWin ? path.join(workspaceFolder, 'vendor', 'protobuf-25.2', 'win', 'tools', 'protoc.exe') :
  //     isNix ? path.join(workspaceFolder, 'vendor', 'protobuf-25.2', 'nix', 'bin', 'protoc') :
  //       path.join(workspaceFolder, 'vendor', 'protobuf-25.2', 'mac', 'bin', 'protoc');
  // await child_spawn(PROTOC_PATH, [
  //   `--cpp_out=${path.join(workspaceFolder, 'src', 'proto')}`,
  //   '--proto_path', path.join(workspaceFolder, 'assets', 'proto'),
  //   'addressbook.proto',
  // ]);

  // await child_spawn(PROTOC_PATH, [
  //   '--encode', 'tutorial.AddressBook',
  //   '--proto_path', path.join(workspaceFolder, 'assets', 'proto'),
  //   'addressbook.proto',
  // ], {
  //   stdin: path.join(workspaceFolder, 'assets', 'proto', 'addressbook.pb'),
  //   stdout: path.join(workspaceFolder, 'assets', 'proto', 'addressbook.bin'),
  // });
};

const clean = async () => {
  await fs.rm(path.join(workspaceFolder, BUILD_PATH), { recursive: true, force: true });
  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH));

  const shaderFiles = await glob(path.join(workspaceFolder, 'assets', 'shaders', '*.spv').replace(/\\/g, '/'));
  for (const shaderFile of shaderFiles) {
    await fs.rm(shaderFile, { force: true });
  }
};

const compile = async (basename) => {
  console.log(`compiling ${basename}...`);
  const absBuild = (...args) => path.join(workspaceFolder, BUILD_PATH, ...args);

  // compile translation units in parallel (N-at-once)
  const main = `src/${basename}.c`;
  const unit_files = [main];
  const dsts = [`${main}.o`];
  for (const u of COMPILER_TRANSLATION_UNITS) {
    for (const file of await glob(path.relative(workspaceFolder, absBuild(u)).replace(/\\/g, '/'))) {
      if (file.includes('Logic.c')) { continue; }
      unit_files.push(file);
      dsts.push(rel(workspaceFolder, BUILD_PATH, `${file}.o`));
    }
  }
  const compileTranslationUnit = async (unit) => {
    const dir = path.relative(process.cwd(), absBuild(path.dirname(unit)));
    await fs.mkdir(dir, { recursive: true });

    const src = rel(workspaceFolder, unit);
    const dst = rel(workspaceFolder, BUILD_PATH, `${unit}.o`);

    let dstExists = false;
    try {
      await fs.access(path.join(BUILD_PATH, dst), fs.constants.F_OK);
      dstExists = true;
    }
    catch (e) {
    }
    if (dstExists) {
      const srcStat = await fs.stat(path.join(BUILD_PATH, src));
      const dstStat = await fs.stat(path.join(BUILD_PATH, dst));
      if (srcStat.mtime < dstStat.mtime) {
        return;
      }
    }

    const is_c = RX_C.test(src);
    await child_spawn((is_c ? C_COMPILER_PATH : CPP_COMPILER_PATH), [
      ...DEBUG_COMPILER_ARGS,
      ...(is_c ? C_COMPILER_ARGS : CPP_COMPILER_ARGS),
      ...(is_c ? C_COMPILER_INCLUDES : CPP_COMPILER_INCLUDES),
      ...C_CONDITIONAL_COMPILER_ARGS(src),
      src,
      '-c',
      '-o', dst,
    ]);

    return dst;
  };
  const objs = [];
  for await (const obj of promiseBatch(CONCURRENCY, unit_files, compileTranslationUnit)) {
    if (obj) {
      objs.push(obj);
    }
  }

  // linker stage
  const executable = `${basename}${isWin ? '.exe' : ''} `;
  let code = 0;
  if (objs.length > 0) {
    code = await child_spawn(C_COMPILER_PATH, [
      ...DEBUG_COMPILER_ARGS,
      //...CPP_COMPILER_ARGS,
      ...C_COMPILER_ARGS,
      ...C_COMPILER_INCLUDES,
      ...LINKER_LIBS,
      ...LINKER_LIB_PATHS,
      ...dsts.filter(s => !s.includes(".pb.")),
      '-o', executable,
    ]);
  }
  console.log("done compiling.");
};

function generateRandomString(length) {
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  let result = '';
  for (let i = 0; i < length; i++) {
    const randomIndex = Math.floor(Math.random() * chars.length);
    result += chars[randomIndex];
  }
  return result;
}

const compile_reload = async (outname) => {
  console.log(`recompiling...`);

  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH, 'tmp'), { recursive: true });

  const absBuild = (...args) => path.join(workspaceFolder, BUILD_PATH, ...args);

  const dsts = [];
  for (const u of COMPILER_TRANSLATION_UNITS) {
    for (const file of await glob(path.relative(workspaceFolder, absBuild(u)).replace(/\\/g, '/'))) {
      if (!ENGINE_ONLY.includes(nixPath(file))) {
        // console.log("===", nixPath(file));
        dsts.push(rel(workspaceFolder, file));
      }
    }
  }

  const unit = 'src/game/Logic.c';
  const dir = path.relative(process.cwd(), absBuild(path.dirname(unit)));
  await fs.mkdir(dir, { recursive: true });

  const src = rel(workspaceFolder, unit);
  const target = outname;
  const dst = rel(workspaceFolder, BUILD_PATH, target);

  // for (const tu of dsts) {
  //   const started = performance.now();
  //   await child_spawn(C_COMPILER_PATH, [
  //     ...DEBUG_COMPILER_ARGS,
  //     // '-ftime-report', // display compile time stats
  //     ...C_COMPILER_ARGS,
  //     // ...C_COMPILER_INCLUDES,
  //     // ...C_CONDITIONAL_COMPILER_ARGS(dsts.join(',')),
  //     // ...LINKER_LIBS,
  //     // ...LINKER_LIB_PATHS,
  //     // '-shared',
  //     // ...dsts.filter(s => !s.includes('.pb.')),
  //     '-c', tu
  //     // '-o', dst,
  //   ]);
  //   const ended = performance.now();
  //   console.log(`compile unit performance. file: ${tu}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`);
  // }

  const ANALYZE = false;
  const ANALYZER = path.join(process.cwd(), 'ClangBuildAnalyzer.exe');
  if (ANALYZE) await child_spawn(ANALYZER, ['--start', 'src/game']);

  const started = performance.now();
  await child_spawn(C_COMPILER_PATH, [
    ...DEBUG_COMPILER_ARGS,
    '-ftime-trace', // display compile time stats
    // '-fsyntax-only',
    ...C_COMPILER_ARGS,
    // ...C_COMPILER_INCLUDES.filter(lib => lib.includes('glm')),
    // ...C_CONDITIONAL_COMPILER_ARGS(dsts.join(',')),
    // ...LINKER_LIBS,
    // ...LINKER_LIB_PATHS,
    '-shared',
    ...dsts.filter(s => !s.includes('.pb.')),
    '-o', dst,
  ]);
  const ended = performance.now();

  if (ANALYZE) await child_spawn(ANALYZER, ['--stop', 'src/game', 'analysis.bin']);
  if (ANALYZE) await child_spawn(ANALYZER, ['--analyze', 'analysis.bin']);

  // swap lib
  try {
    await fs.stat(path.join(workspaceFolder, BUILD_PATH, target));
    const target2 = target.replace('.tmp', '');
    // try {
    //   await fs.rename(path.join(workspaceFolder, BUILD_PATH, target), path.join(workspaceFolder, BUILD_PATH, 'tmp', target2));
    // } catch (e) {
    // }

    if (target != target2) {
      await fs.cp(path.join(workspaceFolder, BUILD_PATH, target), path.join(workspaceFolder, BUILD_PATH, target2));
    }
    console.log(`recompiled. file: ${target2}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`);
    return dst;
  } catch (e) {
    console.log('recompilation failed.', e);
  }
};

const watch = async () => {
  console.log(`watching...`);
  const watcher = fs.watch(path.join(workspaceFolder, 'src', 'game', 'Logic.c'));
  let timer;
  let wait = false;
  for await (const event of watcher) {
    clearTimeout(timer);
    if (!wait) {
      timer = setTimeout(async () => {
        wait = true;
        await compile_reload(`src/game/${generateRandomString(16)}.dll.tmp`);
        wait = false;
      }, 250);
    }
  }
};

const run = async (basename) => {
  const executable = `${basename}${isWin ? '.exe' : ''}`;
  const exePath = path.join(workspaceFolder, BUILD_PATH, executable);

  try {
    await fs.stat(exePath);
  } catch (e) {
    console.log(".exe is missing. probably failed to compile.", e);
    return;
  }

  if (isNix || isMac) {
    // chmod +x
    await fs.chmod(exePath, 0o755);
  }
  if (isMac) {
    await child_spawn('install_name_tool', [
      '-change', '@rpath/libvulkan.1.dylib', `${process.env.HOME} /VulkanSDK/1.3.236.0 / macOS / lib / libvulkan.1.dylib`,
      'Pong_test',
    ]);
    // or
    // export DYLD_LIBRARY_PATH=$HOME/VulkanSDK/1.3.236.0/macOS/lib:$DYLD_LIBRARY_PATH
  }

  await child_spawn(exePath);
}

(async () => {
  const [, , ...cmds] = process.argv;
  loop:
  for (const cmd of cmds) {
    switch (cmd) {
      case 'all':
        all();
        break;
      case 'clean':
        await clean();
        break;
      case 'copy_dlls':
        await copy_dlls();
        break;
      case 'shaders':
        await shaders();
        break;
      case 'protobuf':
        await protobuf();
        break;
      case 'compile_commands':
        await generate_clangd_compile_commands();
        break;
      case 'main':
        await compile(cmd);
        break;
      case 'reload':
        await compile_reload("src/game/Logic.c.dll");
        break;
      case 'watch':
        await watch();
        break;
      case 'help':
      default:
        console.log(`
Mike's hand-rolled build system.

  USAGE:
  node build_scripts\\Makefile.mjs < SUBCOMMAND >

    SUBCOMMANDS:
  all
  Clean, rebuild, and launch the default app.
    clean
    Delete all build output.
    copy_dlls
    Copy dynamic libraries to build directory.
    shaders
    Compile SPIRV shaders with GLSLC.
    protobuf
    Compile protobuf.cc code and.bin data files.
    compile_commands
    Generate the.json file needed for clangd for vscode extension.
    main
    Compile and run the main app
    `);
        break loop;
    }
  }
})();