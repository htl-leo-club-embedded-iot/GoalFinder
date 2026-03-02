/*
 * ===============================================================================
 * (c) HTBLA Leonding 2024 - 2026
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */



import type {Player} from "@/models/player";
import type {WebSocketStore, EventCallback} from "@/stores/websocket";

abstract class Game {
    protected readonly _players: Player[];
    private _isRunning: boolean = false;
    protected wsStore: WebSocketStore | null = null;
    protected hitHandler: EventCallback | null = null;
    protected missHandler: EventCallback | null = null;

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

    public start(wsStore: WebSocketStore): void {
        this.wsStore = wsStore;
        this._isRunning = true;
        wsStore.sendStart();
    }

    public pause(): void {
        this._isRunning = false;
        if (this.wsStore) {
            this.wsStore.sendStop();
            if (this.hitHandler) {
                this.wsStore.off("hit", this.hitHandler);
                this.hitHandler = null;
            }
            if (this.missHandler) {
                this.wsStore.off("miss", this.missHandler);
                this.missHandler = null;
            }
        }
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

    public start(wsStore: WebSocketStore): void {
        if (!this.isRunning) {
            this.hitHandler = () => {
                if (this.hasEnded) return;
                console.log("[ShotChallenge] hit detected");
                this.getSelectedPlayer().addHit();
                this.resetTimer();
                this.selectNewPlayer();
            };

            this.missHandler = () => {
                if (this.hasEnded) return;
                console.log("[ShotChallenge] miss detected");
                this.getSelectedPlayer().addMiss();
                this.resetTimer();
                this.selectNewPlayer();
            };

            wsStore.on("hit", this.hitHandler);
            wsStore.on("miss", this.missHandler);

            // Timer countdown only — no HTTP polling
            this.timerIntervalId = setInterval(() => {
                if (this.hasEnded) return;
                this._timer--;

                if (this._timer <= 0) {
                    this.getSelectedPlayer().addMiss();
                    this.resetTimer();
                    this.selectNewPlayer();
                }
            }, 1000);
        }

        super.start(wsStore);
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
        if (this.selectedPlayerIndex >= this._players.length) {
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

    public start(wsStore: WebSocketStore): void {
        if (!this.isRunning) {
            this.hitHandler = () => {
                if (this.hasEnded) return;
                console.log("[TimedShots] hit detected");
                this.getSelectedPlayer().addHit();
            };

            this.missHandler = () => {
                if (this.hasEnded) return;
                console.log("[TimedShots] miss detected");
                this.getSelectedPlayer().addMiss();
            };

            wsStore.on("hit", this.hitHandler);
            wsStore.on("miss", this.missHandler);

            // Timer countdown only — no HTTP polling
            this.timerIntervalId = setInterval(() => {
                if (this.hasEnded) return;
                this._timer--;

                if (this._timer <= 0) {
                    this.resetTimer();
                    this.selectNewPlayer();
                }
            }, 1000);
        }

        super.start(wsStore);
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
        if (this.selectedPlayerIndex >= this._players.length) {
            this.selectedPlayerIndex = 0;
        }
    }

    public resetTimer(): void {
        this._timer = TimedShotsChallengeGame.PLAY_DURATION;
    }
}

export class FreePlayGame {
    private _hits: number = 0;
    private _misses: number = 0;
    private _isRunning: boolean = false;
    private wsStore: WebSocketStore | null = null;
    private hitHandler: EventCallback | null = null;
    private missHandler: EventCallback | null = null;

    public get hits(): number {
        return this._hits;
    }

    public get misses(): number {
        return this._misses;
    }

    public get isRunning(): boolean {
        return this._isRunning;
    }

    public start(wsStore: WebSocketStore): void {
        if (!this._isRunning) {
            this.wsStore = wsStore;

            this.hitHandler = () => {
                console.log("[FreePlay] hit detected");
                this._hits++;
            };

            this.missHandler = () => {
                console.log("[FreePlay] miss detected");
                this._misses++;
            };

            wsStore.on("hit", this.hitHandler);
            wsStore.on("miss", this.missHandler);
            wsStore.sendStart();
        }

        this._isRunning = true;
    }

    public pause(): void {
        this._isRunning = false;
        if (this.wsStore) {
            this.wsStore.sendStop();
            if (this.hitHandler) {
                this.wsStore.off("hit", this.hitHandler);
                this.hitHandler = null;
            }
            if (this.missHandler) {
                this.wsStore.off("miss", this.missHandler);
                this.missHandler = null;
            }
        }
    }

    public reset(): void {
        this.pause();
        this._hits = 0;
        this._misses = 0;
    }
}