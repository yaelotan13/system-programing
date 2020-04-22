const init = require('./workpool');

const addTask = init('./child.js');

addTask('Yael', (result) => {
  console.log(`Yael: ${result}`);
});
addTask('Adi', (result) => {
  console.log(`Adi: ${result}`);
});
addTask('Daniel', (result) => {
  console.log(`Daniel: ${result}`);
});
addTask('Vova', (result) => {
  console.log(`Vova: ${result}`);
});
// addTask('Nir', (result) => {
//   console.log(`Nir: ${result}`);
// });
// addTask('Liora', (result) => {
//   console.log(`Liora ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });
// addTask('Shieber', (result) => {
//   console.log(`Shieber: ${result}`);
// });