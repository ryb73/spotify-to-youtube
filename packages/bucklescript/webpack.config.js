const path = require('path');

module.exports = {
  entry: {
    index: './lib/js/src/react/index.js',
  },
  output: {
    path: path.join(__dirname, "html/js"),
    filename: '[name].js',
  },
};
