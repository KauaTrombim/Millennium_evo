
const canvas = document.getElementById('cenario');
const ctx = canvas.getContext('2d');

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

var populacao = [];
const numeroDeNaves = 20;

for (let i = 0; i < numeroDeNaves; i++) {
    //Coloca a nave em um lugar aleatório
    let startX = Math.random() * (canvas.width - 50);
    let startY = Math.random() * (canvas.height - 50);
    
    // Velocidade aleatória
    let velX = (Math.random() * 4) - 2;
    if (Math.abs(velX) < 0.1) velX = 0.5; // Garante uma velocidade mínima
    let velY = (Math.random() * 4) - 2;
    if (Math.abs(velY) < 0.1) velY = 0.5; // Garante uma velocidade mínima

    //Insere a nave
    populacao.push(new nave(i, startX, startY, 25, 35, { x: velX, y: velY }));
}
console.log("População criada:", populacao);

function animate() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (const individuo of populacao) {

        individuo.update(canvas, populacao); 
        individuo.draw(ctx);
    }

    requestAnimationFrame(animate);
}

// Inicia o loop de animação
animate();