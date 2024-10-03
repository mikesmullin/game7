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
  // export debug symbols (x86dbg understands both; turn these on when debugging, leave off for faster compile)
  // '-gdwarf', // DWARF (GDB / Linux compatible)
  '-g', /* '-gcodeview',*/ // CodeView (PDB / windbg / Windows compatible)
];
const C_COMPILER_ARGS = [
  '-m64', // generate 64-bit executable 

  // ignore specific warnings
  '-Wno-microsoft-enum-forward-reference',
  '-Wno-deprecated-non-prototype',
];
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
  rel(workspaceFolder, 'src', 'game', '**', '*.c'),
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
    ...(ANALYZE ? ['-ftime-trace'] : []), // display compile time stats
    // '-fsyntax-only',
    ...C_COMPILER_ARGS,
    ...C_COMPILER_INCLUDES.filter(lib => lib.includes('glm')),
    // ...C_CONDITIONAL_COMPILER_ARGS(dsts.join(',')),
    ...LINKER_LIBS,
    ...LINKER_LIB_PATHS,
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
  const watcher = fs.watch(path.join(workspaceFolder, 'src'), { recursive: true });
  let timer;
  let wait = false;
  for await (const event of watcher) {
    console.debug('event', event);
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

const idea = async () => {
  const adjectives = "aback,abaft,abandoned,abashed,aberrant,abhorrent,abiding,abject,ablaze,able,abnormal,aboard,aboriginal,abortive,abounding,abrasive,abrupt,absent,absolute,absorbed,absorbing,abstracted,absurd,abundant,abusive,academic,acceptable,accessible,accidental,acclaimed,accomplished,accurate,aching,acid,acidic,acoustic,acrid,acrobatic,active,actual,actually,adamant,adaptable,addicted,adept,adhesive,adjoining,admirable,admired,adolescent,adorable,adored,advanced,adventurous,affectionate,afraid,aged,aggravating,aggressive,agile,agitated,agonizing,agreeable,ahead,ajar,alarmed,alarming,alcoholic,alert,alienated,alike,alive,all,alleged,alluring,aloof,altruistic,amazing,ambiguous,ambitious,amiable,ample,amuck,amused,amusing,anchored,ancient,ancient,angelic,angry,angry,anguished,animated,annoyed,annoying,annual,another,antique,antsy,anxious,any,apathetic,appetizing,apprehensive,appropriate,apt,aquatic,arctic,arid,aromatic,arrogant,artistic,ashamed,aspiring,assorted,assured,astonishing,athletic,attached,attentive,attractive,auspicious,austere,authentic,authorized,automatic,available,avaricious,average,awake,aware,awesome,awful,awkward,axiomatic,babyish,back,bad,baggy,barbarous,bare,barren,bashful,basic,batty,bawdy,beautiful,beefy,befitting,belated,belligerent,beloved,beneficial,bent,berserk,best,better,bewildered,bewitched,big,big-hearted,billowy,biodegradable,bite-sized,biting,bitter,bizarre,black,black-and-white,bland,blank,blaring,bleak,blind,blissful,blond,bloody,blue,blue-eyed,blushing,bogus,boiling,bold,bony,boorish,bored,boring,bossy,both,bouncy,boundless,bountiful,bowed,brainy,brash,brave,brawny,breakable,breezy,brief,bright,brilliant,brisk,broad,broken,bronze,brown,bruised,bubbly,bulky,bumpy,buoyant,burdensome,burly,bustling,busy,buttery,buzzing,cagey,calculating,callous,calm,candid,canine,capable,capital,capricious,carefree,careful,careless,caring,cautious,cavernous,ceaseless,celebrated,certain,changeable,charming,cheap,cheeky,cheerful,cheery,chemical,chief,childlike,chilly,chivalrous,chubby,chunky,circular,clammy,classic,classy,clean,clear,clear-cut,clever,cloistered,close,closed,cloudy,clueless,clumsy,cluttered,coarse,coherent,cold,colorful,colorless,colossal,colossal,combative,comfortable,common,compassionate,competent,complete,complex,complicated,composed,concerned,concrete,condemned,condescending,confused,conscious,considerate,constant,contemplative,content,conventional,convincing,convoluted,cooing,cooked,cool,cooperative,coordinated,corny,corrupt,costly,courageous,courteous,cowardly,crabby,crafty,craven,crazy,creamy,creative,creepy,criminal,crisp,critical,crooked,crowded,cruel,crushing,cuddly,cultivated,cultured,cumbersome,curious,curly,curved,curvy,cut,cute,cylindrical,cynical,daffy,daily,damaged,damaging,damp,dangerous,dapper,dapper,daring,dark,darling,dashing,dazzling,dead,deadly,deadpan,deafening,dear,dearest,debonair,decayed,deceitful,decent,decimal,decisive,decorous,deep,deeply,defeated,defective,defenseless,defensive,defiant,deficient,definite,delayed,delectable,delicate,delicious,delightful,delirious,demanding,demonic,dense,dental,dependable,dependent,depraved,depressed,deranged,descriptive,deserted,despicable,detailed,determined,devilish,devoted,didactic,different,difficult,digital,dilapidated,diligent,dim,diminutive,dimpled,dimwitted,direct,direful,dirty,disagreeable,disastrous,discreet,discrete,disfigured,disguised,disgusted,disgusting,dishonest,disillusioned,disloyal,dismal,dispensable,distant,distinct,distorted,distraught,distressed,disturbed,divergent,dizzy,domineering,dopey,doting,double,doubtful,downright,drab,draconian,drafty,drained,dramatic,dreary,droopy,drunk,dry,dual,dull,dull,dusty,dutiful,dynamic,dysfunctional,each,eager,early,earnest,earsplitting,earthy,easy,easy-going,eatable,economic,ecstatic,edible,educated,efficacious,efficient,eight,elaborate,elastic,elated,elderly,electric,elegant,elementary,elfin,elite,elliptical,emaciated,embarrassed,embellished,eminent,emotional,empty,enchanted,enchanting,encouraging,endurable,energetic,enlightened,enormous,enraged,entertaining,enthusiastic,entire,envious,envious,equable,equal,equatorial,erect,erratic,essential,esteemed,ethereal,ethical,euphoric,evanescent,evasive,even,evergreen,everlasting,every,evil,exalted,exasperated,excellent,excitable,excited,exciting,exclusive,exemplary,exhausted,exhilarated,exotic,expensive,experienced,expert,extensive,extra-large,extraneous,extra-small,extroverted,exuberant,exultant,fabulous,faded,failing,faint,fair,faithful,fake,fallacious,false,familiar,famous,fanatical,fancy,fantastic,far,faraway,far-flung,far-off,fascinated,fast,fat,fatal,fatherly,faulty,favorable,favorite,fearful,fearless,feeble,feigned,feisty,feline,female,feminine,fertile,festive,few,fickle,fierce,filthy,fine,finicky,finished,firm,first,firsthand,fitting,five,fixed,flagrant,flaky,flamboyant,flashy,flat,flawed,flawless,flickering,flimsy,flippant,floppy,flowery,flufy,fluid,flustered,fluttering,foamy,focused,fond,foolhardy,foolish,forceful,foregoing,forgetful,forked,formal,forsaken,forthright,fortunate,four,fragile,fragrant,frail,frank,frantic,frayed,free,freezing,french,frequent,fresh,fretful,friendly,frightened,frightening,frigid,frilly,frivolous,frizzy,front,frosty,frothy,frozen,frugal,fruitful,frustrating,full,fumbling,fumbling,functional,funny,furry,furtive,fussy,future,futuristic,fuzzy,gabby,gainful,gamy,gaping,gargantuan,garrulous,gaseous,gaudy,general,general,generous,gentle,genuine,ghastly,giant,giddy,gifted,gigantic,giving,glamorous,glaring,glass,gleaming,gleeful,glib,glistening,glittering,gloomy,glorious,glossy,glum,godly,golden,good,good-natured,goofy,gorgeous,graceful,gracious,grand,grandiose,grandiose,granular,grateful,gratis,grave,gray,greasy,great,greedy,green,gregarious,grey,grieving,grim,grimy,gripping,grizzled,groovy,gross,grotesque,grouchy,grounded,growing,growling,grown,grubby,gruesome,grumpy,guarded,guiltless,guilty,gullible,gummy,gusty,guttural,habitual,hairy,half,half,hallowed,halting,handmade,handsome,handsomely,handy,hanging,hapless,happy,happy-go-lucky,hard,hard-to-find,harebrained,harmful,harmless,harmonious,harsh,hasty,hateful,haunting,heady,healthy,heartbreaking,heartfelt,hearty,heavenly,heavy,hefty,hellish,helpful,helpless,hesitant,hidden,hideous,high,highfalutin,high-level,high-pitched,hilarious,hissing,historical,hoarse,holistic,hollow,homeless,homely,honest,honorable,honored,hopeful,horrible,horrific,hospitable,hot,huge,hulking,humble,humdrum,humiliating,humming,humongous,humorous,hungry,hurried,hurt,hurtful,hushed,husky,hypnotic,hysterical,icky,icy,ideal,ideal,idealistic,identical,idiotic,idle,idolized,ignorant,ill,illegal,ill-fated,ill-informed,illiterate,illustrious,imaginary,imaginative,immaculate,immaterial,immediate,immense,imminent,impartial,impassioned,impeccable,imperfect,imperturbable,impish,impolite,important,imported,impossible,impractical,impressionable,impressive,improbable,impure,inborn,incandescent,incomparable,incompatible,incompetent,incomplete,inconclusive,inconsequential,incredible,indelible,indolent,industrious,inexpensive,inexperienced,infamous,infantile,infatuated,inferior,infinite,informal,innate,innocent,inquisitive,insecure,insidious,insignificant,insistent,instinctive,instructive,insubstantial,intelligent,intent,intentional,interesting,internal,international,intrepid,intrigued,invincible,irate,ironclad,irresponsible,irritable,irritating,itchy,jaded,jagged,jam-packed,jaunty,jazzy,jealous,jittery,jobless,joint,jolly,jovial,joyful,joyous,jubilant,judicious,juicy,jumbled,jumbo,jumpy,jumpy,junior,juvenile,kaleidoscopic,kaput,keen,key,kind,kindhearted,kindly,klutzy,knobby,knotty,knowing,knowledgeable,known,kooky,kosher,labored,lackadaisical,lacking,lame,lame,lamentable,languid,lanky,large,last,lasting,late,laughable,lavish,lawful,lazy,leading,leafy,lean,learned,left,legal,legitimate,lethal,level,lewd,light,lighthearted,likable,like,likeable,likely,limited,limp,limping,linear,lined,liquid,literate,little,live,lively,livid,living,loathsome,lone,lonely,long,longing,long-term,loose,lopsided,lost,loud,loutish,lovable,lovely,loving,low,lowly,loyal,lucky,ludicrous,lumbering,luminous,lumpy,lush,lustrous,luxuriant,luxurious,lying,lyrical,macabre,macho,mad,maddening,made-up,madly,magenta,magical,magnificent,majestic,major,makeshift,male,malicious,mammoth,maniacal,many,marked,married,marvelous,masculine,massive,material,materialistic,mature,meager,mealy,mean,measly,meaty,medical,mediocre,medium,meek,melancholy,mellow,melodic,melted,memorable,menacing,merciful,mere,merry,messy,metallic,mighty,mild,military,milky,mindless,miniature,minor,minty,minute,miscreant,miserable,miserly,misguided,mistaken,misty,mixed,moaning,modern,modest,moist,moldy,momentous,monstrous,monthly,monumental,moody,moral,mortified,motherly,motionless,mountainous,muddled,muddy,muffled,multicolored,mundane,mundane,murky,mushy,musty,mute,muted,mysterious,naive,nappy,narrow,nasty,natural,naughty,nauseating,nautical,near,neat,nebulous,necessary,needless,needy,negative,neglected,negligible,neighboring,neighborly,nervous,nervous,new,next,nice,nice,nifty,nimble,nine,nippy,nocturnal,noiseless,noisy,nonchalant,nondescript,nonsensical,nonstop,normal,nostalgic,nosy,notable,noted,noteworthy,novel,noxious,null,numb,numberless,numerous,nutritious,nutty,oafish,obedient,obeisant,obese,oblivious,oblong,obnoxious,obscene,obsequious,observant,obsolete,obtainable,obvious,occasional,oceanic,odd,oddball,offbeat,offensive,official,oily,old,old-fashioned,omniscient,one,onerous,only,open,opposite,optimal,optimistic,opulent,orange,orderly,ordinary,organic,original,ornate,ornery,ossified,other,our,outgoing,outlandish,outlying,outrageous,outstanding,oval,overconfident,overcooked,overdue,overjoyed,overlooked,overrated,overt,overwrought,painful,painstaking,palatable,pale,paltry,panicky,panoramic,parallel,parched,parsimonious,partial,passionate,past,pastel,pastoral,pathetic,peaceful,penitent,peppery,perfect,perfumed,periodic,perky,permissible,perpetual,perplexed,personal,pertinent,pesky,pessimistic,petite,petty,petty,phobic,phony,physical,picayune,piercing,pink,piquant,pitiful,placid,plain,plaintive,plant,plastic,plausible,playful,pleasant,pleased,pleasing,plucky,plump,plush,pointed,pointless,poised,polished,polite,political,pompous,poor,popular,portly,posh,positive,possessive,possible,potable,powerful,powerless,practical,precious,premium,present,present,prestigious,pretty,previous,pricey,prickly,primary,prime,pristine,private,prize,probable,productive,profitable,profuse,proper,protective,proud,prudent,psychedelic,psychotic,public,puffy,pumped,punctual,pungent,puny,pure,purple,purring,pushy,pushy,putrid,puzzled,puzzling,quack,quaint,quaint,qualified,quarrelsome,quarterly,queasy,querulous,questionable,quick,quickest,quick-witted,quiet,quintessential,quirky,quixotic,quixotic,quizzical,rabid,racial,radiant,ragged,rainy,rambunctious,rampant,rapid,rare,rash,raspy,ratty,raw,ready,real,realistic,reasonable,rebel,recent,receptive,reckless,recondite,rectangular,red,redundant,reflecting,reflective,regal,regular,reliable,relieved,remarkable,reminiscent,remorseful,remote,repentant,repulsive,required,resolute,resonant,respectful,responsible,responsive,revolving,rewarding,rhetorical,rich,right,righteous,rightful,rigid,ringed,ripe,ritzy,roasted,robust,romantic,roomy,rosy,rotating,rotten,rotund,rough,round,rowdy,royal,rubbery,ruddy,rude,rundown,runny,rural,rustic,rusty,ruthless,sable,sad,safe,salty,same,sandy,sane,sarcastic,sardonic,sassy,satisfied,satisfying,savory,scaly,scandalous,scant,scarce,scared,scary,scattered,scented,scholarly,scientific,scintillating,scornful,scratchy,scrawny,screeching,second,secondary,second-hand,secret,secretive,sedate,seemly,selective,self-assured,selfish,self-reliant,sentimental,separate,serene,serious,serpentine,several,severe,shabby,shadowy,shady,shaggy,shaky,shallow,shameful,shameless,sharp,shimmering,shiny,shivering,shocked,shocking,shoddy,short,short-term,showy,shrill,shut,shy,sick,silent,silky,silly,silver,similar,simple,simplistic,sincere,sinful,single,six,sizzling,skeletal,skillful,skinny,sleepy,slight,slim,slimy,slippery,sloppy,slow,slushy,small,smarmy,smart,smelly,smiling,smoggy,smooth,smug,snappy,snarling,sneaky,sniveling,snobbish,snoopy,snotty,sociable,soft,soggy,solid,somber,some,sophisticated,sordid,sore,sorrowful,soulful,soupy,sour,sour,spanish,sparkling,sparse,special,specific,spectacular,speedy,spherical,spicy,spiffy,spiky,spirited,spiritual,spiteful,splendid,spooky,spotless,spotted,spotty,spry,spurious,squalid,square,squeaky,squealing,squeamish,squiggly,stable,staid,stained,staking,stale,standard,standing,starchy,stark,starry,statuesque,steadfast,steady,steel,steep,stereotyped,sticky,stiff,stimulating,stingy,stormy,stout,straight,strange,strict,strident,striking,striped,strong,studious,stunning,stunning,stupendous,stupid,sturdy,stylish,subdued,submissive,subsequent,substantial,subtle,suburban,successful,succinct,succulent,sudden,sugary,sulky,sunny,super,superb,superficial,superior,supportive,supreme,sure-footed,surprised,suspicious,svelte,swanky,sweaty,sweet,sweltering,swift,sympathetic,symptomatic,synonymous,taboo,tacit,tacky,talented,talkative,tall,tame,tan,tangible,tangy,tart,tasteful,tasteless,tasty,tattered,taut,tawdry,tearful,tedious,teeming,teeny,teeny-tiny,telling,temporary,tempting,ten,tender,tense,tenuous,tepid,terrible,terrific,tested,testy,thankful,that,therapeutic,these,thick,thin,thinkable,third,thirsty,this,thorny,thorough,those,thoughtful,thoughtless,threadbare,threatening,three,thrifty,thundering,thunderous,tidy,tight,tightfisted,timely,tinted,tiny,tired,tiresome,toothsome,torn,torpid,total,tough,towering,tragic,trained,tranquil,trashy,traumatic,treasured,tremendous,triangular,tricky,trifling,trim,trite,trivial,troubled,truculent,true,trusting,trustworthy,trusty,truthful,tubby,turbulent,twin,two,typical,ubiquitous,ugliest,ugly,ultimate,ultra,unable,unaccountable,unarmed,unaware,unbecoming,unbiased,uncomfortable,uncommon,unconscious,uncovered,understated,understood,undesirable,unequal,unequaled,uneven,unfinished,unfit,unfolded,unfortunate,unhappy,unhealthy,uniform,unimportant,uninterested,unique,united,unkempt,unknown,unlawful,unlined,unlucky,unnatural,unpleasant,unrealistic,unripe,unruly,unselfish,unsightly,unsteady,unsuitable,unsung,untidy,untimely,untried,untrue,unused,unusual,unwelcome,unwieldy,unwitting,unwritten,upbeat,uppity,upright,upset,uptight,urban,usable,used,used,useful,useless,utilized,utopian,utter,uttermost,vacant,vacuous,vagabond,vague,vain,valid,valuable,vapid,variable,various,vast,velvety,venerated,vengeful,venomous,verdant,verifiable,versed,vexed,vibrant,vicious,victorious,vigilant,vigorous,villainous,violent,violet,virtual,virtuous,visible,vital,vivacious,vivid,voiceless,volatile,voluminous,voracious,vulgar,wacky,waggish,waiting,wakeful,wan,wandering,wanting,warlike,warm,warmhearted,warped,wary,wasteful,watchful,waterlogged,watery,wavy,weak,wealthy,weary,webbed,wee,weekly,weepy,weighty,weird,welcome,well-documented,well-groomed,well-informed,well-lit,well-made,well-off,well-to-do,well-worn,wet,which,whimsical,whirlwind,whispered,whispering,white,whole,wholesale,whopping,wicked,wide,wide-eyed,wiggly,wild,willing,wilted,winding,windy,winged,wiry,wise,wistful,witty,wobbly,woebegone,woeful,womanly,wonderful,wooden,woozy,wordy,workable,worldly,worn,worried,worrisome,worse,worst,worthless,worthwhile,worthy,wrathful,wretched,writhing,wrong,wry,xenophobic,yawning,yearly,yellow,yellowish,yielding,young,youthful,yummy,zany,zealous,zesty,zigzag,zippy,zonked";
  const nouns = "apple,arm,ball,banana,bat,bed,bike,bird,book,book,boy,bun,cake,can,cap,car,cat,chin,clam,class,clover,club,corn,cow,crayon,crib,crow,crowd,crown,cub,cup,dad,day,desk,dime,dirt,dog,doll,dress,dust,fan,fang,feet,field,flag,flower,fog,game,girl,gun,hall,hat,heat,hen,hill,home,horn,hose,jar,joke,juice,kite,kite,lake,maid,man,map,mask,meal,meat,men,mice,milk,mint,mom,moon,morning,mother,name,nest,nose,pan,pear,pen,pencil,pet,pie,pig,plant,pot,rain,rat,river,road,rock,room,rose,seed,shape,shoe,shop,show,sink,snail,snake,snow,soda,sofa,son,star,step,stew,stove,straw,string,summer,sun,swing,table,tank,team,tent,test,toe,toes,tree,tub,van,vest,water,wing,winter,woman,women";
  const select = (s) => {
    const arr = s.split(',');
    return arr[Math.floor(Math.random() * arr.length)];
  };
  console.log(`${select(adjectives)} ${select(nouns)}`);
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
      case 'clang':
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
      case 'idea':
        await idea();
        break;
      case 'help':
      default:
        console.log(`
Mike's hand-rolled build system.

USAGE:
node build_scripts\\Makefile.mjs < SUBCOMMAND >

SUBCOMMANDS:
  all        Clean, rebuild, and launch the default app.
  clean      Delete all build output.
  copy_dlls  Copy dynamic libraries to build directory.
  shaders    Compile SPIRV shaders with GLSLC.
  protobuf   Compile protobuf.cc code and.bin data files.
  clang      Generate the.json file needed for clangd for vscode extension.
  main       Compile and run the main app
  idea       Surprise generator
  `);
        break loop;
    }
  }
})();