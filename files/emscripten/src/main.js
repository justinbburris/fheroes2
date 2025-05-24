import './style.css'
import { setupGame } from './game.js'

document.querySelector('#app').innerHTML = `
  <div class="min-h-screen bg-[#f5deb3] flex flex-col">
    <!-- Game Canvas -->
    <canvas id="canvas" class="fixed inset-0 w-full h-full z-10" width="640" height="400"></canvas>

    <!-- Upload Interface -->
    <div id="uploader" class="fixed inset-0 z-20 flex items-center justify-center bg-black/50 backdrop-blur-sm">
      <div class="bg-white p-8 rounded-[10px] shadow-xl border-2 border-[#8b4513] max-w-lg w-full mx-4">
        <h2 class="text-2xl font-bold text-[#8b4513] text-center mb-6">Welcome to fheroes2</h2>
        <label class="block w-full">
          <div class="flex flex-col items-center justify-center p-8 border-2 border-dashed border-[#8b4513] rounded-[10px] cursor-pointer hover:bg-[#f5deb3]/20 transition-colors">
            <svg class="w-12 h-12 text-[#8b4513] mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12"/>
            </svg>
            <span class="text-lg text-[#2c1810]">Click to select your Heroes 2 folder</span>
            <input type="file" directory="directory" multiple webkitdirectory="webkitdirectory" id="files" class="hidden" />
          </div>
        </label>
      </div>
    </div>

    <!-- Launcher Interface -->
    <div id="launcher" class="fixed inset-0 z-20 hidden flex items-center justify-center bg-black/50 backdrop-blur-sm">
      <div class="bg-white p-8 rounded-[10px] shadow-xl border-2 border-[#8b4513] max-w-lg w-full mx-4">
        <h2 class="text-2xl font-bold text-[#8b4513] text-center mb-6">Game Ready</h2>
        <div class="flex flex-col space-y-4">
          <button id="startGame" class="w-full px-6 py-3 bg-[#8b4513] text-white rounded-[10px] hover:bg-[#cd853f] transition-colors font-medium">
            Start Game
          </button>
          <button id="deleteFiles" class="w-full px-6 py-3 bg-red-600 text-white rounded-[10px] hover:bg-red-700 transition-colors font-medium">
            Delete Files
          </button>
        </div>
      </div>
    </div>

    <!-- Progress Bar -->
    <div id="progress" class="fixed inset-0 z-30 hidden flex items-center justify-center bg-black/50 backdrop-blur-sm">
      <div class="bg-white p-8 rounded-[10px] shadow-xl border-2 border-[#8b4513] max-w-lg w-full mx-4">
        <h2 class="text-2xl font-bold text-[#8b4513] text-center mb-6">Loading Game</h2>
        <div class="w-full bg-gray-200 rounded-full h-4">
          <div id="progressBar" class="bg-[#8b4513] h-4 rounded-full transition-all duration-300" style="width: 0%"></div>
        </div>
        <p id="progressText" class="text-center text-[#2c1810] mt-4">0%</p>
      </div>
    </div>
  </div>
`

// Initialize the game functionality
setupGame(document.querySelector('#canvas'))
