import { existsSync } from 'node:fs';

const required = [
  'architecture/Layer.hpp',
  'architecture/Domain.hpp',
  'architecture/SecurityLevel.hpp',
  'architecture/ModuleInfo.hpp',
  'architecture/IModule.hpp',
  'docs/EXOTIC_MASTER_ARCHITECTURE.md',
  'docs/EXOTIC_MODULE_CONTRACT.md',
  'docs/ARCHITECTURE_VALIDATOR.md',
  'docs/KERNEL.md'
];

let ok = true;
console.log('Exotic Architecture Validator v0.1');

for (const file of required) {
  if (existsSync(file)) {
    console.log('[OK] ' + file);
  } else {
    console.log('[MISSING] ' + file);
    ok = false;
  }
}

process.exit(ok ? 0 : 1);
