export class Player {
    private readonly _name: string;
    private _hits: number = 0;
    private _misses: number = 0;

    constructor(name: string) {
        this._name = name;
    }

    get name() {
        return this._name;
    }

    get hits() {
        return this._hits;
    }

    get misses() {
        return this._misses;
    }

    public addHit() {
        this._hits++;
    }

    public addMiss() {
        this._misses++;
    }
}