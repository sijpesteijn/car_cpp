import { DetailComponent } from './detail.component';
import { NoLifelineError } from './no_lifeline/no_lifeline.component';
import { Routes } from '@angular/router';
import {RaceComponent} from "./race.component";

export const race_routes: Routes = [
    { path: '', component: RaceComponent },
    { path: 'race', component: RaceComponent }
];