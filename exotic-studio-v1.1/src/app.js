const overlay = document.getElementById('overlay');
const openPalette = document.getElementById('openPalette');
const input = document.getElementById('paletteInput');
const selectedTitle = document.getElementById('selectedTitle');

function showPalette(){
  overlay.classList.add('open');
  setTimeout(() => input && input.focus(), 10);
}
function hidePalette(){
  overlay.classList.remove('open');
}

openPalette?.addEventListener('click', showPalette);
overlay?.addEventListener('click', (e) => { if(e.target === overlay) hidePalette(); });

window.addEventListener('keydown', (e) => {
  if((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'k') {
    e.preventDefault();
    showPalette();
  }
  if(e.key === 'Escape') hidePalette();
});

document.querySelectorAll('.node').forEach(node => {
  node.addEventListener('click', () => {
    selectedTitle.textContent = node.dataset.name || node.innerText;
  });
});
