export class Team {
    private readonly _name: string = "";
    private _goals = 0;

    constructor(name: string) {
        this._name = name;
    }

    get name() {
        return this._name;
    };

    get goals() {
        return this._goals;
    };

    scoreGoal() {
        this._goals++;
    };
}