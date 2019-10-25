    console.error('ERROR: npm is known not to run on Node.js ' + process.version)
    console.error("You'll need to upgrade to a newer version in order to use this")
    console.error('version of npm. You can find the latest version at https://nodejs.org/')
    process.exit(1)