# Website

This website is built using [Docusaurus 2](https://docusaurus.io/), a modern static website generator.

The original layout is retained. Current documentation lives in `docs/` and
describes the EvoPixel fork 5.0.4; `versioned_docs/` retains the older upstream
references. The version selector uses `versions.json`, while the current
sidebar is generated from the document folders and `sidebar_position` fields.
Platform setup belongs in `docs/basics/`, alongside the other setup guides.

### Installation

```
$ corepack yarn install --immutable
```

Run these commands from this directory. Its original lockfile uses the Yarn 3
format; this documentation project pins Yarn 3.8.7 for Node.js 20 compatibility,
independently of the library's Yarn Classic lockfile. Corepack selects the version
declared in this directory's `package.json`.

### Local Development

```
$ yarn start
```

This command starts a local development server and opens up a browser window. Most changes are reflected live without having to restart the server.

### Build

```
$ yarn build
```

This command generates static content into the `build` directory and can be served using any static contents hosting service.

### Deployment

The upstream `rntp.dev` website remains the original project's documentation;
changes in this fork do not deploy there. Before deploying a separate site,
configure its own `url`, `baseUrl` and search index. The inherited automatic
deployment workflow is disabled for this fork. Do not deploy over the source
`main` branch; local builds do not create or push any Git branch.

Using SSH:

```
$ USE_SSH=true yarn deploy
```

Not using SSH:

```
$ GIT_USER=<Your GitHub username> yarn deploy
```

If you are using GitHub pages for hosting, this command is a convenient way to build the website and push to the `gh-pages` branch.
