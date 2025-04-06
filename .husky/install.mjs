// Skip Husky install in production and CI
try {
    const { execSync } = await import('child_process')
    execSync('npx husky', { stdio: 'inherit' })
} catch (error) {
    console.log('Optional: run `npm run install-git-hooks` to setup pre-commit hooks')
    // Silently exit - husky is optional
    process.exit(0)
}
