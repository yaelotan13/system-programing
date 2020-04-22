const { fork } = require('child_process');
const os = require('os');

function init(modulePath) {
  const available = [];
  const tasks = [];

  for (let i = 0; i < os.cpus().length; i += 1) {
    const newProcess = fork(modulePath);
    available.push(newProcess);

    newProcess.on('exit', () => {
      console.log('in on exit');
      available.find((cur, index) => {
        if (cur.pid === newProcess.pid) {
          available.splice(index, 1);
        }
      });
      available.push(fork(modulePath));
    });
  }

  function addTask(arg, cb) {
    const newTask = {
      arg,
      cb,
    };

    if (available.length > 0) {
      const curProcess = available.shift();
      curProcess.send(newTask.arg);

      curProcess.on('message', (result) => {
        console.log(available.length);
        available.push(curProcess);
        newTask.cb(result);

        if (tasks.length > 0) {
          const curProcess = available.shift();
          const curTask = tasks.shift();

          curProcess.send(curTask.arg);
          newTask.cb = curTask.cb;
        }
      });

      return;
    }

    tasks.push(newTask);
  }

  if (available.length === 4) {
    const chosenProcess = available[2];

    console.log('in sending signal');
    chosenProcess.kill('SIGUSR2');
  }

  return addTask;
}

module.exports = init;
