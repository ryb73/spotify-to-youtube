const path              = require("path"),
      CopyWebpackPlugin = require("copy-webpack-plugin"),
      webpack           = require("webpack");

module.exports = {
    target: "electron-renderer",

    entry: {
        index: "./lib/js/src/react/index.js",
    },

    output: {
        path: path.resolve("./html"),
        filename: "js/[name].js",
    },

    module: {
        loaders: [{
            // https://github.com/bloomberg/bucklescript/issues/1653
            // This is a really bad solution but will work for now
            test: /\.js$/,
            loader: "string-replace-loader",
            query: {
                search: "(\\W)Promise\\.all\\(([\\w\\s,]+)\\)",
                replace: "$1Promise.all([$2])",
                flags: "g"
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
        }, {
            from: "src/**/*.js",
            to: path.resolve("./lib/js/[path][name].js")
        }])
    ],

    devtool: "source-map",
};
