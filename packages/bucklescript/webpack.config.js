const path              = require("path"),
      CopyWebpackPlugin = require("copy-webpack-plugin"),
      webpack           = require("webpack");

module.exports = {
    target: "electron-renderer",

    entry: {
        index: "./lib/js/src/react/index.js",
    },

    output: {
        path: path.join(__dirname, "html"),
        filename: "js/[name].js",
    },

    module: {
        loaders: [{
            // This is a hacky way to get remotes to work. This is the only way I could think
            // of that didn't result in a bunch of duplicated code
            test: /remote.+\.js$/,
            loader: "string-replace-loader",
            query: {
                search: "(\\W)require",
                replace: "$1require('electron').remote.require",
                flags: "g",
                strict: true
            }
        }]
    },

    plugins: [
        new webpack.DefinePlugin({ "global.GENTLY": false }),

        new CopyWebpackPlugin([{
            from: "node_modules/font-awesome/css",
            to: "css"
        }, {
            from: "node_modules/font-awesome/fonts",
            to: "fonts"
        }])
    ],

    devtool: "source-map",
};
