class nave {
    constructor(id, x, y, width, height, velocity) {
        //Propriedades
        this.id = id;
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.velocity = velocity;
        this.colisoes = 0;

        //Parâmetros de desvio
        this.velocidadeBase = Math.sqrt(velocity.x ** 2 + velocity.y ** 2) || 2;
        this.forcaDeVirada = 0.05; //Ajuste para desvio
        this.zonaDeRepulsaoParedes = 90; //Distancia da parede
        this.zonaDeRepulsaoNaves = 1.5 * Math.sqrt(width**2 + height**2);  // "Bolha" de espaço pessoal de cada nave
        this.forcaDeSeparacaoNaves = 1.2; // Quão forte elas se repelem, constante

        this.image = new Image();
        this.image.src = "../src/ind.png";
        this.angle = 0;
        this.imageLoaded = false;
        this.image.onload = () => { this.imageLoaded = true; };
    }

    update(canvas, todasAsNaves) {
        // Vetor resultante das forças sobre a nave
        let ajusteTotal = { x: 0, y: 0 };

        // Força de desvio das paredes
        let direcaoDesejadaParedes = { x: this.velocity.x, y: this.velocity.y };
        if (this.x < this.zonaDeRepulsaoParedes) {
            direcaoDesejadaParedes.x = this.velocidadeBase;
        } else if (this.x > canvas.width - this.zonaDeRepulsaoParedes) {
            direcaoDesejadaParedes.x = -this.velocidadeBase;
        }
        if (this.y < this.zonaDeRepulsaoParedes) {
            direcaoDesejadaParedes.y = this.velocidadeBase;
        } else if (this.y > canvas.height - this.zonaDeRepulsaoParedes) {
            direcaoDesejadaParedes.y = -this.velocidadeBase;
        }
        ajusteTotal.x += (direcaoDesejadaParedes.x - this.velocity.x);
        ajusteTotal.y += (direcaoDesejadaParedes.y - this.velocity.y);

        // Força de desvio de naves
        let forcaDeSeparacao = { x: 0, y: 0 };
        for (const outraNave of todasAsNaves) {
            if (outraNave === this) continue; // Não se compara consigo mesma

            const dx = this.x - outraNave.x;
            const dy = this.y - outraNave.y;
            const distancia = Math.sqrt(dx * dx + dy * dy);

            // Se a outra nave estiver dentro da zona de repulsão...
            if (distancia > 0 && distancia < this.zonaDeRepulsaoNaves) {
                // Calcula um vetor de repulsão que aponta para longe da outra nave.
                // A força é inversamente proporcional à distância (mais forte quanto mais perto).
                let forcaX = dx / distancia;
                let forcaY = dy / distancia;
                forcaDeSeparacao.x += forcaX;
                forcaDeSeparacao.y += forcaY;
            }
        }
        ajusteTotal.x += forcaDeSeparacao.x * this.forcaDeSeparacaoNaves;
        ajusteTotal.y += forcaDeSeparacao.y * this.forcaDeSeparacaoNaves;


        // Aplica a resultante na velocidade
        // Aplica uma pequena parte do ajuste total à velocidade atual
        this.velocity.x += ajusteTotal.x * this.forcaDeVirada;
        this.velocity.y += ajusteTotal.y * this.forcaDeVirada;

        // Normaliza a velocidade para manter a nave em velocidade constante
        const magnitude = Math.sqrt(this.velocity.x ** 2 + this.velocity.y ** 2) || 1;
        this.velocity.x = (this.velocity.x / magnitude) * this.velocidadeBase;
        this.velocity.y = (this.velocity.y / magnitude) * this.velocidadeBase;

        // Atualiza a posição da nave
        this.x += this.velocity.x;
        this.y += this.velocity.y;
        
        // Atualiza o ângulo visual da imagem
        this.angle = Math.atan2(this.velocity.y, this.velocity.x) + Math.PI / 2 + Math.PI;
    }

    draw(ctx) {
        if (!this.imageLoaded) return;
        ctx.save();
        ctx.translate(this.x + this.width / 2, this.y + this.height / 2);
        ctx.rotate(this.angle);
        ctx.drawImage(this.image, -this.width / 2, -this.height / 2, this.width, this.height);
        ctx.restore();
    }
    
}