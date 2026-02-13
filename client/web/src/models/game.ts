import type {Player} from "@/models/player";

const API_URL = "/api"

abstract class Game {
    protected readonly _players: Player[];
    private _isRunning: boolean = false;

    constructor() {
        this._players = [];
    }

    public get isRunning(): boolean {
        return this._isRunning;
    }

    public get players(): Player[] {
        return [...this._players];
    }

    public get sortedPlayers() {
        return this._players.slice().sort((a, b) => {
            const diffA = a.hits - a.misses;
            const diffB = b.hits - b.misses;
            return diffB - diffA;
        });
    }

    public addPlayer(player: Player): void {
        if (!this._players.some(p => p.name === player.name)) {
            this._players.push(player);
        }
    }

    public removePlayer(playerIndex: number): void {
        this._players.splice(playerIndex, 1);
    }

    public addHitToPlayer(playerIndex: number): void {
        this._players[playerIndex].addHit();

    }

    public addMissToPlayer(playerIndex: number): void {
        this._players[playerIndex].addMiss();
    }

    public start(): void {
        this._isRunning = true;
    }

    public pause(): void {
        this._isRunning = false;
    }

    public abstract reset(): void;
}

export class ShotChallengeGame extends Game {
    private static readonly PLAY_DURATION: number = 60;

    public _timer: number = 0;
    public hasEnded: boolean = false;
    private timerIntervalId: number = -1;
    public selectedPlayerIndex: number = 0;

    public get timer(): number {
        return this._timer;
    }

    public getSelectedPlayer(): Player {
        return this._players[this.selectedPlayerIndex];
    }

    public constructor() {
        super();
        this.resetTimer();
    }

    public removePlayer(playerIndex: number): void {
        super.removePlayer(playerIndex);

        this.reset();
    }

    public async start(): Promise<void> {
        if(!this.isRunning) {
            this.timerIntervalId = setInterval(async () => {
                if (this.hasEnded) return;
                this._timer--;

                if (this.hasEnded) return;
                const newHitsData = await fetch(`${API_URL}/hits`, {method: "GET"});
                const newHits = parseInt(await newHitsData.text());

                if (this.hasEnded) return;
                const newMissesData = await fetch(`${API_URL}/misses`, {method: "GET"});
                const newMisses = parseInt(await newMissesData.text());

                if(this.hasEnded) return;
                if(newHits > 0) {
                    console.log("[ShotChallenge] hits detected:", newHits);
                    for(let i = 0; i < newHits; i++) {
                        this.getSelectedPlayer().addHit();
                        this.resetTimer();
                        this.selectNewPlayer();
                    }
                } else if(newMisses > 0) {
                    console.log("[ShotChallenge] misses detected:", newMisses);
                    for(let i = 0; i < newMisses; i++) {
                        this.getSelectedPlayer().addMiss();
                        this.resetTimer();
                        this.selectNewPlayer();
                    }
                }

                if(this._timer <= 0) {
                    this.getSelectedPlayer().addMiss();
                    this.resetTimer();
                    this.selectNewPlayer();
                }
            }, 1000);
        }

        super.start();
    }

    public pause(): void {
        clearInterval(this.timerIntervalId);

        super.pause();
    }

    public reset(): void {
        this.selectedPlayerIndex = 0;
        this.pause();
        this.resetTimer();
        this.hasEnded = false;
    }

    public selectNewPlayer(): void {
        this.selectedPlayerIndex++;

        if(this.selectedPlayerIndex >= this._players.length) {
            this.selectedPlayerIndex = 0;
        }
    }

    public resetTimer(): void {
        this._timer = ShotChallengeGame.PLAY_DURATION;
    }
}

export class TimedShotsChallengeGame extends Game {
    private static readonly PLAY_DURATION: number = 120;

    public _timer: number = 0;
    public hasEnded: boolean = false;
    private timerIntervalId: number = -1;
    public selectedPlayerIndex: number = 0;

    public get timer(): number {
        return this._timer;
    }

    public getSelectedPlayer(): Player {
        return this._players[this.selectedPlayerIndex];
    }

    public constructor() {
        super();
        this.resetTimer();
    }

    public removePlayer(playerIndex: number): void {
        super.removePlayer(playerIndex);

        this.reset();
    }

    public async start(): Promise<void> {
        if(!this.isRunning) {
            this.timerIntervalId = setInterval(async () => {
                if (this.hasEnded) return;
                this._timer--;

                if (this.hasEnded) return;
                const newHitsData = await fetch(`${API_URL}/hits`, {method: "GET"});
                const newHits = parseInt(await newHitsData.text());

                if (this.hasEnded) return;
                const newMissesData = await fetch(`${API_URL}/misses`, {method: "GET"});
                const newMisses = parseInt(await newMissesData.text());

                if(this.hasEnded) return;
                if(newHits > 0) {
                    console.log("[TimedShots] hits detected:", newHits);
                    for(let i = 0; i < newHits; i++) {
                        this.getSelectedPlayer().addHit();
                    }
                }

                if(newMisses > 0) {
                    console.log("[TimedShots] misses detected:", newMisses);
                    for(let i = 0; i < newMisses; i++) {
                        this.getSelectedPlayer().addMiss();
                    }
                }

                if(this._timer <= 0) {
                    this.resetTimer();
                    this.selectNewPlayer();
                }
            }, 1000);
        }

        super.start();
    }

    public pause(): void {
        clearInterval(this.timerIntervalId);

        super.pause();
    }

    public reset(): void {
        this.selectedPlayerIndex = 0;
        this.pause();
        this.resetTimer();
        this.hasEnded = false;
    }

    public selectNewPlayer(): void {
        this.selectedPlayerIndex++;

        if(this.selectedPlayerIndex >= this._players.length) {
            this.selectedPlayerIndex = 0;
        }
    }

    public resetTimer(): void {
        this._timer = TimedShotsChallengeGame.PLAY_DURATION;
    }
}