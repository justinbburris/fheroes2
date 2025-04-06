// Skip Husky install in production and CI
try {
    const husky = (await import('husky')).default
    console.log(husky())
} catch (error) {
    console.log('Optional: run `npm run install-git-hooks` to setup pre-commit hooks')
    process.exit(0)
}
