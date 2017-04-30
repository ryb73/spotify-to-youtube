const path               = require("path"),
      CopyWebpackPlugin  = require("copy-webpack-plugin");

module.exports = {
    entry: {
        index: "./lib/js/src/react/index.js",
    },

    output: {
        path: path.join(__dirname, "html"),
        filename: "js/[name].js",
    },

    plugins: [
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
