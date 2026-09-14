import { existsSync, readFileSync } from 'node:fs';

const requiredFiles = [
  'README.md',
  'platform.manifest.json',
  'CoreRuntime/CMakeLists.txt',
  'CoreRuntime/ARCHITECTURE.md',
  'CoreRuntime/include/exotic/core/runtime.hpp',
  'CoreRuntime/include/exotic/core/crypto.hpp',
  'CoreRuntime/include/exotic/core/status_api.hpp',
  'CoreRuntime/src/runtime.cpp',
  'CoreRuntime/src/crypto.cpp',
  'CoreRuntime/src/status_api.cpp',
  'CoreRuntime/app/status_server.cpp',
  'CoreRuntime/tests/runtime_tests.cpp',
  'CoreRuntime/tests/status_api_tests.cpp'
];

const allowedMaturity = new Set(['Verified', 'Implemented', 'Proposed', 'Speculative']);
const requiredPlanes = ['control', 'intelligence', 'execution', 'evidence', 'experience'];
const requiredEndpoints = ['/health', '/version', '/capabilities', '/events', '/public/status'];

let ok = true;
const fail = (message) => {
  console.log('[FAIL] ' + message);
  ok = false;
};
const pass = (message) => console.log('[OK] ' + message);

console.log('EXOTIC Architecture Validator v0.2');

for (const file of requiredFiles) {
  if (existsSync(file)) pass(file);
  else fail('missing required file: ' + file);
}

if (existsSync('platform.manifest.json')) {
  try {
    const manifest = JSON.parse(readFileSync('platform.manifest.json', 'utf8'));

    if (manifest.name === 'EXOTIC') pass('manifest identity');
    else fail('manifest.name must be EXOTIC');

    if (typeof manifest.version === 'string' && manifest.version.startsWith('0.2.')) pass('manifest v0.2 version');
    else fail('manifest.version must be 0.2.x during the Core v0.2 release line');

    const planes = new Set(manifest.architecture?.planes ?? []);
    for (const plane of requiredPlanes) {
      if (planes.has(plane)) pass('plane: ' + plane);
      else fail('missing architecture plane: ' + plane);
    }

    if (manifest.architecture?.verification_gate === true) pass('verification gate declared');
    else fail('architecture.verification_gate must be true');

    if (manifest.architecture?.event_hash === 'sha256') pass('SHA-256 event evidence declared');
    else fail('architecture.event_hash must be sha256');

    const maturityRecords = [
      ...(manifest.core_capabilities ?? []),
      ...(manifest.runtime_features ?? []),
      ...(manifest.endpoints ?? [])
    ];
    for (const record of maturityRecords) {
      if (!allowedMaturity.has(record.maturity)) fail(`invalid maturity for ${record.id ?? record.path ?? 'record'}: ${record.maturity}`);
    }
    if (maturityRecords.length > 0 && ok) pass('maturity labels');

    const endpoints = new Map((manifest.endpoints ?? []).map((entry) => [entry.path, entry]));
    for (const endpoint of requiredEndpoints) {
      if (endpoints.has(endpoint)) pass('endpoint contract: ' + endpoint);
      else fail('missing endpoint contract: ' + endpoint);
    }

    const publicStatus = endpoints.get('/public/status');
    if (publicStatus?.visibility === 'public') pass('public status visibility');
    else fail('/public/status must be explicitly public');

    const rawPaths = ['/health', '/version', '/capabilities', '/events'];
    const rawEndpoints = rawPaths.map((path) => endpoints.get(path));
    if (rawEndpoints.every((entry) => entry?.visibility === 'internal')) pass('raw evidence endpoints internal');
    else fail('raw Core endpoints must be explicitly internal');

    if (requiredEndpoints.every((path) => endpoints.get(path)?.maturity === 'Implemented')) pass('status API endpoints implemented');
    else fail('Core v0.2 status API endpoints must be Implemented');

    if (manifest.network?.status_bind === '127.0.0.1') pass('status API loopback bind declared');
    else fail('network.status_bind must be 127.0.0.1');

    if (manifest.network?.remote_exposure === 'disabled') pass('remote exposure disabled');
    else fail('network.remote_exposure must be disabled for v0.2');
  } catch (error) {
    fail('manifest parse/validation error: ' + error.message);
  }
}

process.exit(ok ? 0 : 1);
