import * as React from 'react';
import { Button, ButtonGroup } from '@mui/material';

function Sidebar({ resetGame, stepPlaybackMove}) {
     
  return <>
  <div id="sideBarContainer">
    <ButtonGroup variant="contained" size='small' orientation='vertical'>
        <Button onClick={resetGame}>Reset</Button>
        <Button onClick={stepPlaybackMove}> &gt; </Button>
        <Button> Fog </Button>
    </ButtonGroup>
    </div>
    </>

}

export default Sidebar;