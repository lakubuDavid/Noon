let count = 0;

const btn = document.querySelector('#btn');
btn.addEventListener('click', (ev) => {
    count++;
    btn.textContent = ` Count :: ${count}`;
})

