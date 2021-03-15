const {runCommand} = require("../utils/utils");


describe('yu-judge-core for different languages', () => {
  test('c', () => {
    runCommand('yarn judge')
  })
});