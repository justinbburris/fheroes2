// File system constants
const S_IFMT = 61440;  /* 0x0170000 type of file */
const S_IFDIR = 16384; /* 0x0040000 directory */

// Environment configuration
const ENV = {
    FHEROES2_DATA: '/fheroes2/data',
    HOME: '/fheroes2'
};

// Required game files
const REQUIRED_FILES = ['data/HEROES2.AGG', 'data/HEROES2X.AGG'];
const WHITELISTED_DIRS = ['data', 'heroes2', 'maps', 'music'];

export function setupGame(canvas) {
    // Prevent right-click context menu on canvas
    canvas.oncontextmenu = event => event.preventDefault();

    // Initialize Module object
    window.Module = {
        preRun: [
            setupEnvironment,
            setupFileSystem,
            setupFileUpload
        ],
        clearInput,
        mkdirWithParents,
        clearPath,
        startGame,
        deleteFiles,
        setStatus,
        canvas
    };

    // Load the game script
    const script = document.createElement('script');
    script.src = './fheroes2.js';
    document.head.appendChild(script);
}

// Environment setup
function setupEnvironment() {
    Object.assign(window.ENV, ENV);
}

// File system setup
function setupFileSystem() {
    addRunDependency('syncfs');
    FS.mkdir('/fheroes2');
    FS.mount(IDBFS, { root: '/' }, '/fheroes2');
    FS.syncfs(true, (err) => {
        if (err) throw err;
        try {
            if (Object.keys(FS.lookupPath(`${ENV.FHEROES2_DATA}`)).length === 0) return;
            showLauncher();
        }
        catch (ignore) {
            FS.mkdir(`${ENV.FHEROES2_DATA}`);
        }
    });
}

// File upload setup
function setupFileUpload() {
    document.querySelector('input[type=file]').addEventListener('input', handleFileUpload);
}

// File upload handler
function handleFileUpload(e) {
    const files = [...e.target.files];

    // Validation checks
    if (files.length === 1) {
        return showError('Zip archives are not supported yet, sorry');
    }
    if (files.length > 3000) {
        return showError('Wrong directory, or your maps collection is really awesome.');
    }

    // Check for required files
    const hasRequiredFiles = files.filter(({ webkitRelativePath: path }) =>
        REQUIRED_FILES.some(rq => path.toLowerCase().endsWith(rq.toLowerCase()))).length === 2;

    if (!hasRequiredFiles) {
        return showError('Wrong directory - missing required game files');
    }

    // Process files
    const validFiles = files.filter(file => {
        const [fileName, ...path] = file.webkitRelativePath.split('/').reverse();
        if (fileName.startsWith('.')) return false;
        return path
            .map(folder => folder.toLowerCase())
            .some(folder => WHITELISTED_DIRS.includes(folder));
    });

    uploadFiles(validFiles);
}

// File upload process
function uploadFiles(files) {
    const uploaded = [];

    files.forEach(file => {
        const [fileName, ...path] = file.webkitRelativePath.split('/').reverse();
        const [ignore, ...relativePath] = path.reverse();

        mkdirWithParents(`${ENV.FHEROES2_DATA}/${relativePath.join('/')}`);

        const reader = new FileReader();
        reader.addEventListener('error', console.error);
        reader.addEventListener('loadend', ({ target: { result } }) => {
            FS.writeFile(
                `${ENV.FHEROES2_DATA}/${relativePath.join('/')}/${fileName}`,
                new Uint8Array(result),
                { encoding: 'binary' }
            );

            uploaded.push(file);
            updateProgress(uploaded.length, files.length);

            if (uploaded.length === files.length) {
                FS.syncfs(false, err => {
                    if (err) return showError('Failed to sync file system');
                    showLauncher();
                });
            }
        });

        reader.readAsArrayBuffer(file);
    });
}

// Progress update
function updateProgress(current, total) {
    const progress = Math.round((current / total) * 100);
    const progressBar = document.getElementById('progressBar');
    const progressText = document.getElementById('progressText');

    if (progressBar && progressText) {
        progressBar.style.width = `${progress}%`;
        progressText.textContent = `${progress}%`;
    }
}

// UI state management
function showError(message) {
    clearInput();
    alert(message);
}

function showLauncher() {
    document.querySelector('#uploader').style.display = 'none';
    document.querySelector('#launcher').style.display = 'flex';
}

function showProgress() {
    document.querySelector('#progress').style.display = 'flex';
}

function hideProgress() {
    document.querySelector('#progress').style.display = 'none';
}

// Module methods
function clearInput(msg = '') {
    document.querySelector('input[type=file]').value = null;
    msg && alert(msg);
}

function mkdirWithParents(path) {
    const parts = path.split('/');
    try {
        FS.lookupPath(parts.join('/'))
    }
    catch (ignore) {
        parts.reduce((p, part) => {
            p = [...p, part];
            if (!part) return p;
            try {
                FS.lookupPath(p.join('/'))
            }
            catch (ignore) {
                FS.mkdir(p.join('/'))
            }
            return p;
        }, []);
    }
}

function clearPath(path) {
    try {
        Object.keys(FS.lookupPath(path).node.contents).forEach(node =>
            (FS.stat(`${path}/${node}`).mode & S_IFMT) === S_IFDIR
                ? clearPath(`${path}/${node}`)
                : FS.unlink(`${path}/${node}`));
        FS.rmdir(`${path}`);
    } catch (ignore) {}
}

function startGame() {
    removeRunDependency('syncfs');
    document.querySelector('#uploader').style.display = 'none';
    document.querySelector('#launcher').style.display = 'none';
}

function deleteFiles() {
    if (!confirm('Are you sure you want to delete all game files?')) return;

    clearPath(ENV.FHEROES2_DATA);
    FS.syncfs(false, () => {});
    clearInput();
    document.querySelector('#uploader').style.display = 'flex';
    document.querySelector('#launcher').style.display = 'none';
}

function setStatus(status) {
    const dlProgressRE = /(?<progress>\d+)\/(?<total>\d+)/ig;

    if (!status || !dlProgressRE.test(status)) {
        if ((status ?? '').startsWith('Downloading data')) {
            showProgress();
        }
        return;
    }

    dlProgressRE.lastIndex = 0;
    const {groups: {progress, total}} = [...status.matchAll(dlProgressRE)][0];
    const done = Math.round(progress / total * 100);

    updateProgress(progress, total);

    if (done === 100) {
        hideProgress();
    }
}
