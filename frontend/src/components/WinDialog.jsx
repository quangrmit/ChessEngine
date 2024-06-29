import { Dialog, DialogContent, DialogContentText, DialogActions, Button } from '@mui/material'

function WinDialog({ open, resetGame, cancleGame, message }) {
    return <>
    <Dialog
        open={open}
        onClose={resetGame}
      >
        <DialogContent>
          <DialogContentText>
            {message}
          </DialogContentText>
        </DialogContent>
        <DialogActions>
          <Button onClick={resetGame}>Restart Game</Button>
          <Button onClick={cancleGame} autoFocus>Cancel</Button>
        </DialogActions>
      </Dialog>
    </>
}

export default WinDialog